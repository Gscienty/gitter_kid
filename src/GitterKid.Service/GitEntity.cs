using System;
using System.IO;
using System.IO.Compression;
using System.Text;
using ComponentAce.Compression.Libs.zlib;

namespace GitterKid.Service
{
    public abstract class GitEntity
    {
        protected string _entityFilePath;
        protected string _repositoryPath;

        public string Signture { get; private set; }
        public int Size { get; private set; }
        public string ContentType { get; private set; }
        protected byte[] Body { get; private set; }
        public bool IsExist => File.Exists(this._entityFilePath);

        internal GitEntity() { }

        internal abstract void Initialize();

        internal static T Load<T>(string repositoryPath, string signture) where T : GitEntity, new()
        {
            T result = new T();

            result.SetPath(repositoryPath, signture);
            result.Initialize();

            return result;
        }

        private void SetPath(string repositoryPath, string signture)
        {
            this.Signture = signture;
            this._repositoryPath = repositoryPath;
            this._entityFilePath = $"{repositoryPath}/objects/{signture.Substring(0, 2)}/{signture.Substring(2)}";

            (byte[] Header, byte[] Body) result = this.GetFileContent();

            this.Body = result.Body;
        }

        protected (byte[] Header, byte[] Body) GetFileContent()
        {
            MemoryStream inflatedStream = this.getInflateStream(this.GetDeflateContent());
            byte[] inflatedBuffer = inflatedStream.GetBuffer();
            
            MemoryStream headerStream = new MemoryStream();
            MemoryStream bodyStream = new MemoryStream();
            bool isHeaderPart = true;
            for (int i = 0; i < inflatedStream.Length; i++)
            {
                if (inflatedBuffer[i] == 0 && isHeaderPart)
                {
                    isHeaderPart = false;
                    continue;
                }

                if (isHeaderPart)
                {
                    headerStream.WriteByte(inflatedBuffer[i]);
                }
                else
                {
                    bodyStream.WriteByte(inflatedBuffer[i]);
                }
            }

            inflatedStream.Close();
            inflatedStream.Dispose();

            return (headerStream.ToArray(), bodyStream.ToArray());
        }

        private MemoryStream getInflateStream(byte[] fileBuffer)
        {

            ZStream inflateStream = new ZStream();
            inflateStream.inflateInit();

            inflateStream.avail_in = fileBuffer.Length;
            inflateStream.next_in = fileBuffer;

            int inflateMiddleBufferSize = 1024;
            byte[] inflateMiddleBuffer = new byte[inflateMiddleBufferSize];

            MemoryStream result = new MemoryStream();
            do
            {
                inflateStream.avail_out = inflateMiddleBufferSize;
                inflateStream.next_out = inflateMiddleBuffer;

                inflateStream.inflate(zlibConst.Z_NO_FLUSH);
                int readedLength = inflateMiddleBufferSize - inflateStream.avail_out;
                result.Write(inflateMiddleBuffer, 0, readedLength);
            } while (inflateStream.avail_out == 0);

            inflateStream.inflateEnd();

            return result;
        }

        private byte[] GetDeflateContent()
        {
            byte[] result;
            using (FileStream inFile = new FileStream(this._entityFilePath, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                using (MemoryStream fileStream = new MemoryStream())
                {        
                    inFile.CopyTo(fileStream);
                    result = fileStream.ToArray();

                    fileStream.Close();
                }
                inFile.Close();
            }

            return result;
        }
    }
}