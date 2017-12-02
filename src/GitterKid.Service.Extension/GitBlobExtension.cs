using System;
using System.Collections.Generic;

namespace GitterKid.Service.Extension
{
    public static class GitBlobExtension
    {
        public static (int AddedCount, int DeletedCount) Compare(this GitBlob currentBlob, GitBlob originBlob)
        {
            int addedCount = 0;
            int deletedCount = 0;

            IReadOnlyList<string> originLines = originBlob.GetLines();
            IReadOnlyList<string> currentLines = currentBlob.GetLines();

            int currentLineFindStartOffset = 0;

            for (
                int originLineOffset = 0;
                originLineOffset < originLines.Count;
                originLineOffset++)
            {
                if (currentLineFindStartOffset >= currentLines.Count)
                {
                    deletedCount += originLines.Count - originLineOffset;
                    break;
                }

                string originLine = originLines[originLineOffset];

                int findLineOffset = -1;
                for (
                    int currentLineOffset = currentLineFindStartOffset;
                    currentLineOffset < currentLines.Count;
                    currentLineOffset++)
                {
                    if (originLine.Equals(currentLines[currentLineOffset]))
                    {
                        findLineOffset = currentLineOffset;
                        break;
                    }
                }
                if (findLineOffset == -1)
                {
                    deletedCount++;
                }
                else
                {
                    addedCount += findLineOffset - 1 - currentLineFindStartOffset;
                    currentLineFindStartOffset = findLineOffset + 1;
                }
            }

            if (currentLineFindStartOffset < currentLines.Count)
            {
                addedCount += currentLines.Count - currentLineFindStartOffset;
            }

            return (addedCount, deletedCount);
        }
        public static IReadOnlyList<string> GetLines(this GitBlob gitBlob)
            => gitBlob.TextContent.Split(new char[] { '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries);
    }
}