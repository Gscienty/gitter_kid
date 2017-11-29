using System;
using System.Text;
using System.IO;
using System.Collections.Generic;
using System.Collections;

namespace GitterKid.Service
{
    public class GitTreeEntity : GitEntity
    {
        private Dictionary<string, (string Signture, int Type)> _treeContent;

        public int Count => this._treeContent.Count;

        public bool Exist(string name) => this._treeContent.ContainsKey(name);
        public int GetFileType(string name) => this._treeContent[name].Type;

        public T Open<T>(string name) where T : GitEntity, new()
        {
            if (this._treeContent.ContainsKey(name) == false)
            {
                throw new FileNotFoundException(name);
            }

            return GitEntity.Load<T>(this._repositoryPath, this._treeContent[name].Signture);
        }

        public GitTreeEntity()
        {
            this._treeContent = new Dictionary<string, (string Signture, int Type)>();
        }

        internal override void Initialize()
        {
            byte[] fileContent =  this.GetFileContent().Body;

            AnalysisStep analysisStep = AnalysisStep.WantType;

            int currentType = 0;
            string currentName = string.Empty;
            string currentSignture = string.Empty;

            MemoryStream currentSegmentBuilder = new MemoryStream();
            for (int i = 0; i < fileContent.Length; i++)
            {
                switch (analysisStep)
                {
                    case AnalysisStep.WantName:
                        if (fileContent[i] == 0)
                        {
                            // 注：当MemoryStream使用.GetBuffer获得的byte[]，使之GetString之后得到的string
                            // 在MemoryStream flush之后，该字符串会为空。
                            currentName = Encoding.UTF8.GetString(currentSegmentBuilder.ToArray());

                            currentSegmentBuilder.SetLength(0);
                            analysisStep = AnalysisStep.WantSignture;
                        }
                        else
                        {
                            currentSegmentBuilder.WriteByte(fileContent[i]);
                        }
                        break;
                    case AnalysisStep.WantSignture:
                        currentSegmentBuilder.WriteByte(fileContent[i]);

                        if (currentSegmentBuilder.Length == 20)
                        {
                            byte[] buffer = currentSegmentBuilder.GetBuffer();

                            StringBuilder signtureBuilder = new StringBuilder();
                            for (int signtureOffset = 0; signtureOffset < 20; signtureOffset++)
                            {
                                signtureBuilder.Append(buffer[signtureOffset].ToString("x2"));
                            }

                            currentSignture = signtureBuilder.ToString();
                            this._treeContent.Add(currentName, (currentSignture, currentType));
                            currentName = currentSignture = string.Empty;
                            currentType = 0;

                            currentSegmentBuilder.SetLength(0);
                            analysisStep = AnalysisStep.WantType;
                        }
                    break;
                    case AnalysisStep.WantType:
                        if (fileContent[i] == ' ')
                        {
                            analysisStep = AnalysisStep.WantName;
                        }
                        else
                        {
                            currentType *= 10;
                            currentType += fileContent[i] - 48;
                        }
                    break;
                }
            }
        }

        private enum AnalysisStep
        {
            WantType,
            WantName,
            WantSignture
        }
    }
}