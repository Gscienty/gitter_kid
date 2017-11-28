using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace GitterKid.Service
{
    public class RepositoryConfig
    {
        private readonly string _configPath;
        public Dictionary<string, Dictionary<string, string>> Segments { get; private set; }
        
        internal RepositoryConfig(string path)
        {
            this._configPath = $"{path}/config";

            this.Initialize();
        }

        public void Upgrade(string segment, string key, string value)
        {
            this.AddConfig(segment, key, value);

            this.WriteBack();
        }

        public void AddRange(List<(string Segment, string Key, string Value)> configs)
        {
            configs.ForEach(tuple => this.AddConfig(tuple.Segment, tuple.Key, tuple.Value));

            this.WriteBack();
        }

        public void Remove(string segment, string key)
        {
            this.RemoveConfig(segment, key);

            this.WriteBack();
        }

        public void Remove(string segment)
        {
            this.RemoveSegment(segment);

            this.WriteBack();
        }

        private void RemoveConfig(string segment, string key)
        {
            if (this.Segments.ContainsKey(segment) == false)
            {
                return;
            }

            if (this.Segments[segment].ContainsKey(key) == false)
            {
                return;
            }

            this.Segments[segment].Remove(key);
        }

        private void RemoveSegment(string segment)
        {
            if (this.Segments.ContainsKey(segment) == false)
            {
                return ;
            }

            this.Segments.Remove(segment);
        }

        private void AddConfig(string segment, string key, string value)
        {
            if (this.Segments.ContainsKey(segment) == false)
            {
                this.Segments.Add(segment, new Dictionary<string, string>());
            }

            if (this.Segments[segment].ContainsKey(key))
            {
                this.Segments[segment][key] = value;
            }
            else
            {
                this.Segments[segment].Add(key, value);
            }
        }

        private void WriteBack()
        {
            StringBuilder configBuilder = new StringBuilder();

            foreach(var segment in this.Segments)
            {
                configBuilder.AppendLine($"[{segment.Key}]");

                foreach(var configPair in segment.Value)
                {
                    configBuilder.AppendLine($"\t{configPair.Key} = {configPair.Value}");
                }
            }

            byte[] fileContent = Encoding.UTF8.GetBytes(configBuilder.ToString());
            
            if (File.Exists(this._configPath))
            {
                File.Delete(this._configPath);
            }

            using (var writor = File.OpenWrite(this._configPath))
            {
                writor.Write(fileContent, 0, fileContent.Length);
                writor.Close();
            }
        }

        private void Initialize()
        {
            this.Segments = new Dictionary<string, Dictionary<string, string>>();
            string currentSegment = string.Empty;
            foreach(string line in this.ReadConfigFile())
            {
                if (line.StartsWith("\t"))
                {
                    if (string.IsNullOrEmpty(currentSegment))
                    {
                        throw new FormatException("Error config file format.");
                    }
                    string[] pair = line.Substring(1).Split('=');
                    if (pair.Length != 2)
                    {
                        throw new FormatException("Error config file format.");
                    }

                    if (this.Segments.ContainsKey(currentSegment) == false)
                    {
                        this.Segments.Add(currentSegment, new Dictionary<string, string>());
                    }

                    this.Segments[currentSegment].Add(pair[0].Trim(), pair[1].Trim());
                }
                else if (line.StartsWith("["))
                {
                    currentSegment = line.Substring(1, line.Length - 2);
                }
            }
        }

        private IEnumerable<string> ReadConfigFile()
        {
            if (File.Exists(this._configPath) == false)
            {
                throw new FileNotFoundException("Not found config file.");
            }

            return File.ReadAllLines(this._configPath);
        }
    }
}