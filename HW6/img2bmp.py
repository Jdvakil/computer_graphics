from PIL import Image

filename = "argentina_jersey.png"

with Image.open(filename) as image:
    image.convert("RGB").resize(
        (512, 512), Image.Resampling.LANCZOS
    ).save(filename.replace(".png", ".bmp"))