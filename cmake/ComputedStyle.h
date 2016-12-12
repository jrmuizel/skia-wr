#include "LayoutRectOutsets.h"
namespace blink {
class ComputedStyle {
    public:

        void getBorderEdgeInfo(BorderEdge edges[], bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const
        {
            bool horizontal = isHorizontalWritingMode();
            edges[BSTop] = BorderEdge(m_border.borderTopWidth(),
                                      Color(),
                                      m_border.borderTopStyle(),
                                      horizontal || includeLogicalLeftEdge);

	    edges[BSRight] = BorderEdge(m_border.borderRightWidth(),
					Color(),
					m_border.borderRightStyle(),
					!horizontal || includeLogicalRightEdge);

	    edges[BSBottom] = BorderEdge(m_border.borderBottomWidth(),
					 Color(),
					 m_border.borderBottomStyle(),
					 horizontal || includeLogicalRightEdge);

	    edges[BSLeft] = BorderEdge(m_border.borderLeftWidth(),
				       Color(),
				       m_border.borderLeftStyle(),
				       !horizontal || includeLogicalLeftEdge);

        }

        FloatRoundedRect getRoundedInnerBorderFor(const LayoutRect& borderRect, bool includeLogicalLeftEdge = true, bool includeLogicalRightEdge = true) const {
            FloatRoundedRect roundedRect(pixelSnappedIntRect(borderRect));
            if (hasBorderRadius()) {
                FloatRoundedRect::Radii radii = calcRadiiFor(m_border, borderRect.size());
                roundedRect.includeLogicalEdges(radii, isHorizontalWritingMode(), includeLogicalLeftEdge, includeLogicalRightEdge);
                roundedRect.constrainRadii();
            }
            return roundedRect;
        }
        FloatRoundedRect getRoundedInnerBorderFor(const LayoutRect& borderRect,
                                                  const LayoutRectOutsets insets, bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const {
            LayoutRect innerRect(borderRect);
            innerRect.expand(insets);

            FloatRoundedRect roundedRect(pixelSnappedIntRect(innerRect));
            if (hasBorderRadius()) {
                FloatRoundedRect::Radii radii = getRoundedBorderFor(borderRect).getRadii();
                // Insets use negative values.
                radii.shrink(
                             -insets.top(),
                             -insets.bottom(),
                             -insets.left(),
                             -insets.right());
                roundedRect.includeLogicalEdges(radii, isHorizontalWritingMode(), includeLogicalLeftEdge, includeLogicalRightEdge);
            }
            return roundedRect;
        }
        FloatRoundedRect getRoundedBorderFor(const LayoutRect& borderRect,
                                             bool includeLogicalLeftEdge = true,
                                             bool includeLogicalRightEdge = true) const
        {
            FloatRoundedRect roundedRect(pixelSnappedIntRect(borderRect));
            if (hasBorderRadius()) {
                FloatRoundedRect::Radii radii = calcRadiiFor(m_border, borderRect.size());
                roundedRect.includeLogicalEdges(radii, isHorizontalWritingMode(), includeLogicalLeftEdge, includeLogicalRightEdge);
                roundedRect.constrainRadii();
            }
            return roundedRect;
        }

        bool hasBorderRadius() const { return m_hasBorderRadius; }
        bool m_hasBorderRadius;

        bool isHorizontalWritingMode() const { return true; }
        struct BorderData {
            FloatSize m_topLeft;
            FloatSize m_topRight;
            FloatSize m_bottomLeft;
            FloatSize m_bottomRight;
            int m_topWidth;
            int m_bottomWidth;
            int m_leftWidth;
            int m_rightWidth;

            EBorderStyle m_topStyle;
            EBorderStyle m_bottomStyle;
            EBorderStyle m_leftStyle;
            EBorderStyle m_rightStyle;

            int borderTopWidth() const { return m_topWidth; }
            int borderBottomWidth() const { return m_bottomWidth; }
            int borderLeftWidth() const { return m_leftWidth; }
            int borderRightWidth() const { return m_rightWidth; }

            EBorderStyle borderTopStyle() const { return m_topStyle; }
            EBorderStyle borderBottomStyle() const { return m_bottomStyle; }
            EBorderStyle borderLeftStyle() const { return m_leftStyle; }
            EBorderStyle borderRightStyle() const { return m_rightStyle; }

            FloatSize topLeft() const { return m_topLeft; }
            FloatSize topRight() const { return m_topRight; }
            FloatSize bottomLeft() const { return m_bottomLeft; }
            FloatSize bottomRight() const { return m_bottomRight; }
        };

        BorderData  m_border;

        static float floatValueForLength(float x, float max) { if (x > max) return max; return x; }

        static FloatRoundedRect::Radii calcRadiiFor(const BorderData& border, FloatSize size)
        {
            return FloatRoundedRect::Radii(
                                           FloatSize(floatValueForLength(border.topLeft().width(), size.width()),
                                                     floatValueForLength(border.topLeft().height(), size.height())),
                                           FloatSize(floatValueForLength(border.topRight().width(), size.width()),
                                                     floatValueForLength(border.topRight().height(), size.height())),
                                           FloatSize(floatValueForLength(border.bottomLeft().width(), size.width()),
                                                     floatValueForLength(border.bottomLeft().height(), size.height())),
                                           FloatSize(floatValueForLength(border.bottomRight().width(), size.width()),
                                                     floatValueForLength(border.bottomRight().height(), size.height())));
        }
};
}
