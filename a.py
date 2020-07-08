#!/usr/bin/env python3
# coding=utf-8
import os
import math
import sys

from PIL import Image, ImageDraw, ImageFont


def cut(x, l):
    A = []
    a = len (x) // l + 1
    print (a)
    for i in range (a):
        if (i + 1) * l > len (x):
            A.append (x[i * l:])
        else:
            A.append (x[i * l:(i + 1) * l])
    return (A)

if len(sys.argv) != 3:
    print("Usage: a.py <txt> <png>")
    exit(1)
f_in=sys.argv[1]
f_out=sys.argv[2]
words = 23
pix = 22
with open (f_in, "r", encoding='utf-8')as f:
    data = f.read ()
Data = cut (data, words)
Line = len (data) // words + 1
Length = 2 ** math.ceil (math.log (Line * pix, 2))
image = Image.new (mode='RGBA', size=(512, Length))
draw_table = ImageDraw.Draw (im=image)
for i in range (Line):
    draw_table.text (xy=(0, pix * i - 7), text=Data[i], fill='#FFFFFF', font=ImageFont.truetype ('./NotoSansCJK-Medium.ttc', pix),
                         spacing=0, align="left")
image.save (f_out, 'PNG')
exit(Length);