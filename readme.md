<!-- ## update -->
---

+ ## YUV
YUV，分为三个分量，`Y`表示明亮度（Luminance或Luma），也就是灰度值；而`U`和`V` 表示的则是色度（Chrominance或Chroma),
作用是描述影像色彩及饱和度，用于指定像素的颜色。

__YUV格式有两大类__：`planar` 和 `packed`:
`planar`的`YUV`格式，先连续存储所有像素点的`Y`，紧接着存储所有像素点的`U`，随后是所有像素点的`V`。

`packed`的`YUV`格式，每个像素点的`Y,U,V`是连续交错存储的。

```python
    Y Y Y Y Y Y                   
    Y Y Y Y Y Y                  
    Y Y Y Y Y Y                   
    Y Y Y Y Y Y                    
    U U U U U U                        Y U Y V Y U Y V Y U Y V
    U U U U U U                        Y U Y V Y U Y V Y U Y V
    V V V V V V                        Y U Y V Y U Y V Y U Y V
    V V V V V V                        Y U Y V Y U Y V Y U Y V
    - Planar -                          - Packed -
```
**YUV与RGB的转换公式**

```python
B = Y + 1.7790 * (U - 128)
G = Y - 0.3455 * (U - 128) - 0.7169 * (V - 128)
R = Y + 1.4075 * (V - 128)

RGB To YUV

Y = 0.299 * R + 0.587 * G + 0.114 * B
U = (B - Y) / 1.772
v = (R - Y) / 1.402  (U ~(-128 - 127))
```
+ ## YUYV4:2:2格式数据

下面的四个像素为:`[Y0 U0 V0] [Y1 U1 V1] [Y2 U2 V2][Y3 U3 V3]`

存放的码流为:`Y0 U0 Y1 V1 Y2 U2 Y3 V3`

映射出像素点为:`[Y0 U0 V1][Y1 U0 v1] [Y2 U2 V3][Y3 U2 V3]`


+ ## yuv422p 内存分布
```python
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0U1      |
            |...       |   h
            |...       |
            |          |
            +----------+
            |V0V1      |
            |...       |  h
            |...       |
            |          |
            +----------+
                w/2
``` 

+ ## yuv420p 内存分布
```python
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0U1      |
            |...       |   h/2
            |...       |
            |          |
            +----------+
            |V0V1      |
            |...       |  h/2
            |...       |
            |          |
            +----------+
                w/2
```

```python
uint8_t* dst_y = yuvI420;
int dst_y_stride = width;

uint8_t* dst_u = yuvI420 + width * height;
int dst_u_stride = (width + 1) / 2;

uint8_t* dst_v = (yuvI420 + width * height) + dst_u_stride * ((height + 1) / 2);
int dst_v_stride = (width + 1) / 2;

libyuv::ARGBToI420(
                argb.data, 
                width * 4, 
                
                dst_y,
                dst_y_stride, 
                
                dst_u, 
                dst_u_stride, 
                
                dst_v, 
                dst_v_stride, 
                
                width,                                          
                height
            );
```

+ ## demo 
因项目需要，使用`libyuv` 对 `yuyv(yuy2)`格式图像进行处理，主要实现为：

`yuyv(yuy2) ---> I420 ---> I420Scale ---> BGRA`

缩放后图片：

<img style="margin:10px auto;display:block" width=224 src="./img/yuyv2i420_scale.jpg"/>
