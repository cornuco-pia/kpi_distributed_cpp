from PIL import Image

img = Image.open("../01_sysinfo/Civic_machine_assimilator.png").convert("RGB")
width, height = img.size
print("width, height:", width, height)
pixels = list(img.getdata())

with open("02_gui/myimage.h", "w") as f:
    f.write(f"static const int image_width = {width};\n")
    f.write(f"static const int image_height = {height};\n")
    f.write("static const unsigned char image_data[] = {\n")

    for i, (r, g, b) in enumerate(pixels):
        f.write(f"0x{r:02X}, 0x{g:02X}, 0x{b:02X}, ")
        if (i + 1) % 8 == 0:  # wrap lines
            f.write("\n")

    f.write("};\n")
