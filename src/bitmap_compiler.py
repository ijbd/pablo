from PIL import Image
import numpy as np

kw = 'isaac'
filetype = 'jpg'
input_name = '../images/{}.{}'.format(kw,filetype)
output_image_name = '../images/{}_bw.{}'.format(kw,filetype)
output_name = '../prints/{}.pablone'.format(kw)

# open image
img = Image.open(input_name)

# scale image
img.thumbnail((130,130), Image.ANTIALIAS)
img_arr = np.copy(np.asarray(img))

img_height = img_arr.shape[0]
img_width = img_arr.shape[1]


red = img_arr[:,:,0]

if input_name.endswith('.png'):
    a = img_arr[:,:,3]
    red = np.where(a < 100, 255, red)

red = np.where(red > 140, 255, 0)

img_arr[:,:,0] = red
if input_name.endswith('.png'):
    img_arr[:,:,3] = 255

img_arr[:,:,1] = img_arr[:,:,0]
img_arr[:,:,2] = img_arr[:,:,0]


out_img = Image.fromarray(img_arr)
out_img = out_img.rotate(270,expand=1)

# output image
out_img.save(output_image_name)


# write to command file
red = red.T
red = np.flip(red)
print(red.shape)

with open(output_name,'w') as pablone:
    pablone.write('s\n')
    for x in range(len(red[:,0])):
        for y in range(len(red[0,:])):
            if red[x,y] == 0:
                x_coord = x + 5
                y_coord = y + 20
                pablone.write('d {} {}\n'.format(x_coord, y_coord))
    
    pablone.write('e\n')



