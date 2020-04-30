using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.IO;

namespace IconConverter {
    class Program {

        static char toHexDigit(byte data) {
            if (data < 10) {
                return (char)('0' + data);
            } else if (data < 16) {
                return (char)('A' + (data - 10));
            }
            return '\0';
        }

        static string toHexDeclaration(byte data) {
            return "0x" + toHexDigit((byte)((data & 0xF0) >> 4)) + toHexDigit((byte)(data & 0x0F));
        }

        static void Main(string[] args) {
            if (args.Length < 1) {
                Console.WriteLine("Generates a 4-bit palettized bitmap from a full color icon file.");
                Console.WriteLine("Note: This doesn't do anything fancy! It just takes the first 16 colors in your image!");
                Console.WriteLine("      If your image has more than 16 colors, all subsequent colors will be mapped to the first color.");
                Console.WriteLine("Usage: IconConverter <icon filename>");
                return;
            }

            Bitmap icon = (Bitmap) Bitmap.FromFile(args[0]);

            if (icon.Width > 256 || icon.Height > 256) {
                Console.WriteLine("The SNES Jukebox only supports icons up to 256x256 pixels in size.");
                return;
            }

            // Parse colors
            bool tooManyColors = false;
            ushort[] palette = new ushort[16];
            Dictionary<ushort, byte> colorIndexMap = new Dictionary<ushort, byte>();
            byte[][] indices = new byte[icon.Width][];
            for (int y = 0; y < icon.Height; ++y) {
                indices[y] = new byte[icon.Width];

                for (int x = 0; x < icon.Width; ++x) {
                    Color color = icon.GetPixel(x, y);
                    int colorInt = color.ToArgb() & 0x00FFFFFF;
                    ushort shortColor = (ushort)(((colorInt & 0xF80000) >> 8) + ((colorInt & 0x00FC00) >> 5) + ((colorInt & 0xF8) >> 3));
                    if (colorIndexMap.ContainsKey(shortColor)) {
                        indices[y][x] = colorIndexMap[shortColor];
                    } else {
                        if (colorIndexMap.Count < 16) {
                            byte index = (byte)colorIndexMap.Count;
                            colorIndexMap[shortColor] = index;
                            palette[index] = shortColor;
                            indices[y][x] = index;
                        } else {
                            indices[y][x] = 0;
                            tooManyColors = true;
                        }
                    }
                }
            }

            if (tooManyColors) {
                Console.WriteLine("Too many unique colors present. The output image will not look correct.");
            }

            // Build header data
            StringBuilder outputData = new StringBuilder();
            outputData.Append("//" + args[0] + "\n");
            outputData.Append("const PROGMEM uint8_t icon[] = {\n");

            // Image size
            outputData.Append("  // Width, Height\n");
            outputData.Append("  ").Append(toHexDeclaration((byte)icon.Width)).Append(", ").Append(toHexDeclaration((byte)icon.Height)).Append(",\n");

            // Index data
            outputData.Append("  // Tile data (4bpp)\n");
            for (int y = 0; y < indices.Length; ++y) {
                byte[] row = indices[y];
                outputData.Append(" ");
                for (int x = 0; x < row.Length; x += 2) {
                    byte output = (byte)((row[x] & 0x0F) << 4);
                    if (x < row.Length - 1) {
                        output += (byte)(row[x + 1] & 0x0F);
                    }
                    outputData.Append(" ").Append(toHexDeclaration(output)).Append(",");
                }
                outputData.Append("\n");
            }

            // Palette data
            outputData.Append("  // Palette data (2 bytes each, big endian)\n");
            outputData.Append("  ");
            for (int i = 0; i < colorIndexMap.Count; ++i) {
                outputData.Append(toHexDeclaration((byte)((palette[i] & 0xFF00) >> 8)));
                outputData.Append(", ");
                outputData.Append(toHexDeclaration((byte)(palette[i] & 0xFF)));
                if (i < colorIndexMap.Count - 1) {
                    outputData.Append(", ");
                }
            }
            outputData.Append("\n");

            outputData.Append("};\n");

            // Write output file
            File.WriteAllText(args[0] + ".h", outputData.ToString());
        }
    }
}
