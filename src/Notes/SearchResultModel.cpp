#include "SearchResultModel.h"

QString SearchResultModel::generateHighlightPreview(
    const QString &content,
    const QList<KeywordPosition> &positions
    ) {
    QString preview = content.left(200);
    QList<KeywordPosition> sortedPositions = positions;

    // 按起始位置反向排序
    std::sort(sortedPositions.begin(), sortedPositions.end(),
              [](const KeywordPosition &a, const KeywordPosition &b) {
                  return a.charStart > b.charStart;
              });

    for (const KeywordPosition &pos : sortedPositions) {
        if (pos.charStart < preview.length()) {
            int start = pos.charStart;
            int end = pos.charEnd;
            preview.insert(end, "</span>");
            preview.insert(start, "<span style='color: #e74c3c; font-weight: 500;'>");
        }
    }
    return preview;
}
