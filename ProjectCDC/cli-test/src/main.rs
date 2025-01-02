use std::{io::Read, sync::Mutex, thread, time::Duration};
use chrono::Timelike;
use image::{RgbImage, ImageBuffer, Rgb, Rgba};
use imageproc::drawing;
use lazy_static::lazy_static;
use rusttype::{Font, Scale};
use serialport::SerialPort;


//屏幕长宽
const WIDTH:u32 = 240;
const HEIGHT:u32 = 240;

//获取资源文件路径
pub fn get_path() -> String{
    let path = std::env::args().nth(2).unwrap_or(String::from("static/"));
    if path.ends_with('/') || path.ends_with('\\') {
        path
    } else {
        path + "/"
    }
}
//加载字体文件
fn load_font(path: String) -> Font<'static>{
    let mut file = std::fs::File::open(&path).expect(&path);
    let mut data = Vec::new();
    file.read_to_end(&mut data).unwrap();
    Font::try_from_vec(data).unwrap()
}
//静态加载字体
lazy_static! {
    static ref FONT_SARASA: Font<'static> = load_font(get_path() + "sarasa-mono-sc-nerd-regular.ttf");
    static ref IMAGE_CACHE: Mutex<Vec<u8>> = Mutex::new(Vec::with_capacity((240*240*2) as usize));
}

//生成最终的图片序列
fn send_bytes(img: &RgbImage, port: &mut Box<dyn SerialPort>){
    let mut data = IMAGE_CACHE.lock().unwrap();
    data.clear();
    for y in 0..240 {
        for x in 0..240 {
            let color = img.get_pixel(x, y);
            let r = color[0] as u16;
            let g = color[1] as u16;
            let b = color[2] as u16;
            let color = (r-r%8)/8*2048 + (g-g%4)/4*32 + (b-b%8)/8;
            data.push((color / 256) as u8);
            data.push((color % 256) as u8);
        }
    }
    let mut temp : Vec<u8> = vec![];
    //第一包
    temp.push(5);
    temp.push(0);
    temp.push(239);
    temp.push(0);
    temp.push(239);
    port.write_all(&temp[..]).unwrap();

    //剩下的数据包
    let mut sent:usize = 0;

    while sent < data.len() {
        if data.len() - sent >= 64 {
            port.write_all(&data[sent..(sent+64)]).unwrap();
            sent += 64;
        } else {
            let len = data.len() - sent;
            let len = if len > 62 { 62 } else { len };
            sent += len;
            data[sent-1] = len as u8;
            port.write_all(&data[(sent-1)..(sent+len)]).unwrap();
        }
    }
}


fn main() {
    for p in serialport::available_ports().unwrap() {
        println!("{}",p.port_name);
    }
    println!("enter a port to connect:");
    let mut com = String::new();
    std::io::stdin().read_line(&mut com).unwrap();
    let com = com.trim_end();
    println!("opening {}",com);
    let mut com = serialport::new(com, 92160000)
    .timeout(std::time::Duration::from_millis(1))
    .open().unwrap();

    let mut img: RgbImage = ImageBuffer::new(WIDTH, HEIGHT);
    drawing::draw_filled_rect_mut(&mut img,imageproc::rect::Rect::at(0, 0).of_size(WIDTH, HEIGHT),Rgb([100u8,100u8,100u8]));
    let mut img2: RgbImage = ImageBuffer::new(WIDTH, HEIGHT);
    drawing::draw_filled_rect_mut(&mut img2,imageproc::rect::Rect::at(0, 0).of_size(WIDTH, HEIGHT),Rgb([0u8,0u8,0u8]));

    let mut last = true;
    loop {
        // let now = chrono::Local::now();
        // let bgcolor = if last {Rgb([0u8,0u8,0u8])} else {Rgb([100u8,100u8,100u8])};
        // let txtcolor = if last {Rgb([255u8,255u8,255u8])} else {Rgb([255u8,255u8,255u8])};
        last = !last;
        // drawing::draw_filled_rect_mut(&mut img,imageproc::rect::Rect::at(0, 0).of_size(WIDTH, HEIGHT),bgcolor);
        // let now = format!("{:02}:{:02}:{:02}",now.hour(),now.minute(),now.second());
        // println!("{}",now);
        // drawing::draw_text_mut(&mut img, txtcolor, 0,90, Scale {x: 70.0,y: 70.0 }, &FONT_SARASA,&now);
        if last {send_bytes(&img,&mut com);} else {send_bytes(&img2,&mut com);}
        println!("{}",last);
        //thread::sleep(Duration::from_millis(500));
    }
}
