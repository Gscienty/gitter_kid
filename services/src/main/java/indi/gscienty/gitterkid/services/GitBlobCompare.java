package indi.gscienty.gitterkid.services;

import indi.gscienty.gitterkid.repo.GitBlob;

public class GitBlobCompare {
	
	public static int getLinesCount(GitBlob blob) {
		String content = new String(blob.getContent());
		return content.split("\n|\r").length;
	}
	
	/**
	 * 比对两个Blob的增删行数
	 * @param newestBlob 较新的Blob
	 * @param originBlob 原始的Blob
	 * @return 增删行数
	 */
	public static GitBlobCompareResult compare(GitBlob newestBlob, GitBlob originBlob) {
		GitBlobCompareResult result = new GitBlobCompareResult();
		String[] newestBlobLines = GitBlobCompare.getLines(newestBlob);
		String[] originBlobLines = GitBlobCompare.getLines(originBlob);
		
		int newestBlobLineFindStartOffset = 0;
		for (int originLineOffest = 0;
				originLineOffest < originBlobLines.length;
				originLineOffest++) {
			
			if (newestBlobLineFindStartOffset >= newestBlobLines.length) {
				result.accumulationRemovedLine(originBlobLines.length - originLineOffest);
				break;
			}
			
			String originBlobLine = originBlobLines[originLineOffest];
			int findLineOffset = -1;
			for (int newestBlobLineOffset = newestBlobLineFindStartOffset;
					newestBlobLineOffset < newestBlobLines.length;
					newestBlobLineOffset++) {
				
				if (originBlobLine.equals(newestBlobLines[newestBlobLineOffset])) {
					findLineOffset = newestBlobLineOffset;
					break;
				}
			}
			
			if (findLineOffset == -1) {
				result.accumulationRemovedLine(1);
			}
			else {
				result.accumulationAddedLine(findLineOffset - 1 - newestBlobLineFindStartOffset);
				newestBlobLineFindStartOffset = findLineOffset;
			}
		}
		
		if (newestBlobLineFindStartOffset < newestBlobLines.length) {
			result.accumulationAddedLine(newestBlobLines.length - newestBlobLineFindStartOffset);
		}
		
		return result;
	}
	
	/**
	 * 获取Blob中的所有行
	 * @param blob Blob
	 * @return
	 */
	private static String[] getLines(GitBlob blob) {
		String content = new String(blob.getContent());
		return content.split("\n|\r");
	}
}
