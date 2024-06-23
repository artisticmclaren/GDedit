import os
from PIL import Image

os.system("clear")

images = []
im_int = []

d = os.listdir("ids")

ysize=0
xsize=0
obj=0

index=""

print("generating image and index...")
for m in d:
    if int(m.replace(".png",""))==0: continue
    images.append(f"ids/{m}")
    im_int.append(int(m.replace(".png","")))
    if Image.open(f"ids/{m}").size[1]>ysize:
        ysize=Image.open(f"ids/{m}").size[1]
    xsize+=Image.open(f"ids/{m}").size[0]
    obj+=1

im_int.sort()
images.clear()
for a in im_int:
    images.append(f"ids/{a}.png")

total=0

new_im = Image.new('RGBA', (xsize,ysize))
i=0
for im in images:
    o=Image.open(im)
    new_im.paste(o,(total,0))
    index+=f"{total}\n0\n{o.size[0]}\n{o.size[1]}\nnew\n"
    total+=o.size[0]
    i+=1

new_im.save('worlds_worst_spritesheet.png')
print(f"created spritesheet with {obj} objects.")
f = open("spritesheet.txt","w")
f.write(index)
print("written to index.")