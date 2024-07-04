import os, shutil
from PIL import Image

os.system("clear")

images = []
im_int = []

print("object resizer\n")

print("getting all objects in '/ids'...")

d = os.listdir("ids")

for m in d:
    if int(m.replace(".png",""))==0: continue
    images.append(f"ids/{m}")
    im_int.append(int(m.replace(".png","")))

im_int.sort()
images.clear()
for a in im_int:
    images.append(f"ids/{a}.png")
print(f"Found {len(images)} objects.")
while True:
    try:
        a = input("ID of object to resize to 50x50: ")
        if (a=="quit" or a=="exit"):
            quit()
    except KeyboardInterrupt:
        quit()
    try:
        shutil.copy(f"ids/{a}.png",f"ids/{a}.png.backup")
    except:
        print(f"ID {a} does not exist.")
        continue
    print(f"Created backup of ids/{a}.png")
    im = Image.open(f"ids/{a}.png")
    nim = im.resize((50,50))
    nim.save(f"ids/{a}.png")
    print("Please view image.")
    w = input("Restore to backup >")
    if w=="y":
        os.remove(f"ids/{a}.png")
        os.rename(f"ids/{a}.png.backup",f"ids/{a}.png")
    else:
        os.remove(f"ids/{a}.png.backup")
    os.system("clear")