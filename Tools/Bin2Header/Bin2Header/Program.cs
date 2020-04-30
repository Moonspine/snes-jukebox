using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Bin2Header
{
    class Program
    {
        static char toHexChar(int value)
        {
            switch (value)
            {
                case 0: return '0';
                case 1: return '1';
                case 2: return '2';
                case 3: return '3';
                case 4: return '4';
                case 5: return '5';
                case 6: return '6';
                case 7: return '7';
                case 8: return '8';
                case 9: return '9';
                case 10: return 'A';
                case 11: return 'B';
                case 12: return 'C';
                case 13: return 'D';
                case 14: return 'E';
                case 15: return 'F';
                default: return '-';
            }
        }

        static private string toHexString(byte value)
        {
            string result = "";

            byte temp = value;
            for (int i = 0; i < 2; i++)
            {
                result = toHexChar(temp & 0xF) + result;
                temp >>= 4;
            }

            return "0x" + result;
        }

        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Must specify a file to convert!");
                return;
            }

            FileStream file = File.OpenRead(args[0]);

            Console.Write("const PROGMEM uint8_t data[] = {");

            for (long i = 0; i < file.Length; i++)
            {
                if (i > 0)
                    Console.Write(',');

                if (i % 16 == 0)
                {
                    Console.WriteLine();
                    Console.Write("  ");
                }

                if (i % 16 != 0)
                    Console.Write(' ');

                Console.Write(toHexString((byte)file.ReadByte()));
            }

            Console.WriteLine();
            Console.WriteLine("};");

            Console.WriteLine();
            Console.WriteLine("#define DATA_LENGTH " + file.Length.ToString());

            file.Close();
        }
    }
}
