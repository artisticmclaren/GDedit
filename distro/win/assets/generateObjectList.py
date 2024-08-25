import os
from PIL import Image

os.system("clear")

images = []
im_int = []

d = os.listdir("ids")

ysize=30
xsize=0
obj=0
i=0

print("generating image...")
for m in d:
    if int(m.replace(".png",""))==0: continue
    images.append(f"ids/{m}")
    im_int.append(int(m.replace(".png","")))
    if (i==29):
        ysize+=35
        i=0
        xsize=35
    xsize+=35
    obj+=1
    i+=1

im_int.sort()
images.clear()
for a in im_int:
    images.append(f"ids/{a}.png")

total=0

new_im = Image.new('RGBA', (1015,ysize))
i=0
xpos = 0
ypos = 0
for im in images:
    o=Image.open(im)
    if (i==29):
        i=0
        xpos=0
        ypos+=35
    sy = o.height*(30/o.height)
    sx = o.width*(30/o.height)
    r = o.resize((int(sx),int(sy)))
    new_im.paste(r,(xpos,ypos))
    xpos+=35
    i+=1

new_im.save('objects.png')
print(f"created sprite with {obj+1} objects.")