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
img.thumbnail((130*4,130*4), Image.ANTIALIAS)
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

xscale = 2.55
yscale = 2.55
xoffset = 5
yoffset = 5

with open(output_name,'w') as pablone:
    pablone.write('s\n')
    for x in range(len(red[:,0])):
        y = 0
        # loop until all black has been found
        while y < len(red[x]) and np.sum(red[x,y:]==0) != 0:
            # find start 
            while red[x,y] != 0:
                y += 1

            start_x_coord = (140 - xoffset) - x/xscale
            start_y_coord = y/yscale + yoffset

            # check for dot
            if red[x,y+1] != 0:
                pablone.write('d {} {}\n'.format(round(start_x_coord,1), round(start_y_coord,1)))
                y += 1
            # line
            else:
                # find end
                while red[x,y] == 0 and y < len(red[x]):
                    y += 1
                end_x_coord = (140 -xoffset) - x/xscale
                end_y_coord = (y-1)/yscale + yoffset

                pablone.write('l {} {} {} {}\n'.format(round(start_x_coord,1), round(start_y_coord,1), round(end_x_coord,1), round(end_y_coord,1)))            
    
    pablone.write('e\n')



