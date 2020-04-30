using System;
using System.Collections.Generic;
using NAudio.Wave;
using System.Diagnostics;
using System.IO;

namespace wav2br2 {
    class Program {
        struct WaveData {
            public float[][] channelSamples;
            public int sampleRate;
        }

        static WaveData readMp3Data(string filename) {
            string tempFilename = "temp.wav";
            using (Mp3FileReader reader = new Mp3FileReader(filename)) {
                using (WaveStream waveStream = WaveFormatConversionStream.CreatePcmStream(reader)) {
                    WaveFileWriter.CreateWaveFile(tempFilename, waveStream);
                }
            }

            WaveData result = readWaveData(tempFilename);

            File.Delete(tempFilename);

            return result;
        }

        static WaveData readWaveData(string filename) {
            WaveData result = new WaveData();

            using (WaveFileReader reader = new WaveFileReader(filename)) {
                result.channelSamples = new float[reader.WaveFormat.Channels][];
                for (int i = 0; i < result.channelSamples.Length; ++i) {
                    result.channelSamples[i] = new float[reader.SampleCount];
                }
                for (long i = 0; i < reader.SampleCount; ++i) {
                    float[] samples = reader.ReadNextSampleFrame();
                    for (int j = 0; j < samples.Length && j < result.channelSamples.Length; ++j) {
                        result.channelSamples[j][i] = samples[j];
                    }
                }

                result.sampleRate = reader.WaveFormat.SampleRate;
            }

            return result;
        }

        static WaveData convertWaveData(WaveData inputData, ushort outputSampleRate, bool outputStereo) {
            // Step 1: Mix channels
            float[][] mixed;
            if ((inputData.channelSamples.Length > 1 && !outputStereo) || (inputData.channelSamples.Length > 2 && outputStereo)) {
                long sampleCount = inputData.channelSamples[0].LongLength;
                mixed = new float[outputStereo ? 2 : 1][];
                for (int j = 0; j < mixed.Length; ++j) {
                    mixed[j] = new float[sampleCount];
                }
                for (long i = 0; i < sampleCount; ++i) {
                    for (int j = 0; j < inputData.channelSamples.Length; ++j) {
                        mixed[j % mixed.Length][i] += inputData.channelSamples[j][i];
                    }
                    if (mixed.Length > 1) {
                        mixed[0][i] /= ((inputData.channelSamples.Length + 1) / 2);
                        mixed[1][i] /= inputData.channelSamples.Length / 2;
                    } else {
                        mixed[0][i] /= inputData.channelSamples.Length;
                    }
                }
            } else {
                mixed = inputData.channelSamples;
            }

            // Step 2: Adjust sample rate
            WaveData result;
            if (outputSampleRate != inputData.sampleRate) {
                string tempFile1 = "resample1.wav";
                using (WaveFileWriter writer = new WaveFileWriter(tempFile1, WaveFormat.CreateIeeeFloatWaveFormat(inputData.sampleRate, mixed.Length))) {
                    for (long i = 0; i < mixed[0].LongLength; ++i) {
                        for (int j = 0; j < mixed.Length; ++j) {
                            writer.WriteSample(mixed[j][i]);
                        }
                    }
                }

                string tempFile2 = "resample2.wav";
                using (WaveFileReader reader = new WaveFileReader(tempFile1)) {
                    WaveFormat outFormat = new WaveFormat(outputSampleRate, reader.WaveFormat.Channels);
                    using (MediaFoundationResampler resampler = new MediaFoundationResampler(reader, outFormat)) {
                        WaveFileWriter.CreateWaveFile(tempFile2, resampler);
                    }
                }

                result = readWaveData(tempFile2);
                File.Delete(tempFile1);
                File.Delete(tempFile2);
            } else {
                result = new WaveData();
                result.sampleRate = inputData.sampleRate;
                result.channelSamples = mixed;
            }

            return result;
        }

        static string outputTempWaveFile(WaveData waveData, uint channel) {
            if (channel > waveData.channelSamples.Length) {
                throw new InvalidOperationException("Channel index must be less than " + waveData.channelSamples.Length);
            }

            string filename = "temp" + channel + ".wav";

            WaveFileWriter writer = new WaveFileWriter(filename, WaveFormat.CreateCustomFormat(WaveFormatEncoding.Pcm, waveData.sampleRate, 1, waveData.sampleRate * 2, 0, 16));

            float[] samples = waveData.channelSamples[channel];
            for (long i = 0; i < samples.LongLength; ++i) {
                writer.WriteSample(samples[i]);
            }
            writer.Close();

            return filename;
        }

        static string convertToBrr(string waveFilename) {
            string brrFilename = waveFilename + ".brr";

            Process process = new Process();
            process.StartInfo = new ProcessStartInfo("snesbrr.exe", "-e \"" + waveFilename + "\" \"" + brrFilename + "\"");
            process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            process.Start();
            process.WaitForExit();

            return brrFilename;
        }
        
        static void Main(string[] args) {
            // Parse arguments
            if (args.Length < 4) {
                Console.WriteLine("Usage: wave2br2 input.[wav|mp3] outputSampleRate <stereo/mono> output.br2");
                return;
            }

            string inputFilename = args[0];

            ushort outputSampleRate;
            if (!ushort.TryParse(args[1], out outputSampleRate) || outputSampleRate < 16) {
                Console.WriteLine("Output sample rate must be an integer between 16 and 65535, inclusive.");
                return;
            }

            bool outputStereo = args[2].Equals("stereo");

            string outputFilename = args[3];


            // Read samples
            Console.WriteLine("Reading " + inputFilename + "...");
            WaveData waveData;
            if (inputFilename.ToLower().EndsWith(".mp3")) {
                waveData = readMp3Data(inputFilename);
            } else {
                waveData = readWaveData(inputFilename);
            }

            // Convert
            Console.WriteLine("Converting sample data...");
            waveData = convertWaveData(waveData, outputSampleRate, outputStereo);

            // Output wave(s)
            Console.WriteLine("Converting to brr...");
            List<string> waveFiles = new List<string>();
            for (uint i = 0; i < waveData.channelSamples.Length; ++i) {
                waveFiles.Add(outputTempWaveFile(waveData, i));
            }

            // Convert to brr files
            List<string> brrFiles = new List<string>();
            for (int i = 0; i < waveFiles.Count; ++i) {
                brrFiles.Add(convertToBrr(waveFiles[i]));
            }

            // Save BR2 file
            Console.WriteLine("Saving br2 file...");
            byte[][] brrData = new byte[brrFiles.Count][];
            long totalFileSize = 4; // 4 bytes of header
            long maxBrrLength = 0;
            for (int i = 0; i < brrFiles.Count; ++i) {
                brrData[i] = File.ReadAllBytes(brrFiles[i]);
                totalFileSize += brrData[i].LongLength;
                maxBrrLength = Math.Max(maxBrrLength, brrData[i].LongLength);
            }

            byte[] finalFileData = new byte[totalFileSize];
            long offset = 0;

            // Header

            // Version 1
            finalFileData[offset++] = 1;

            // Sample rate
            finalFileData[offset++] = (byte)((waveData.sampleRate & 0xFF00) >> 8);
            finalFileData[offset++] = (byte)(waveData.sampleRate & 0xFF);

            // Channel count
            finalFileData[offset++] = (byte)(waveData.channelSamples.Length);

            // Channel data
            for (long i = 0; i < maxBrrLength; i += 9) {
                for (int j = 0; j < brrData.Length; ++j) {
                    if (brrData[j].Length - i >= 9) {
                        finalFileData[offset++] = brrData[j][i];
                        finalFileData[offset++] = brrData[j][i + 1];
                        finalFileData[offset++] = brrData[j][i + 2];
                        finalFileData[offset++] = brrData[j][i + 3];
                        finalFileData[offset++] = brrData[j][i + 4];
                        finalFileData[offset++] = brrData[j][i + 5];
                        finalFileData[offset++] = brrData[j][i + 6];
                        finalFileData[offset++] = brrData[j][i + 7];
                        finalFileData[offset++] = brrData[j][i + 8];
                    } else {
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                        finalFileData[offset++] = 0;
                    }
                }
            }

            // Write data
            File.WriteAllBytes(outputFilename, finalFileData);

            // Clean up temp files
            Console.WriteLine("Cleaning up temp files...");
            foreach (string filename in waveFiles) {
                File.Delete(filename);
            }
            foreach (string filename in brrFiles) {
                File.Delete(filename);
            }

            Console.WriteLine("Done!");
        }
    }
}
