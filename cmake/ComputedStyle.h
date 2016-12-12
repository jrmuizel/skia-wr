#include "LayoutRectOutsets.h"
namespace blink {
class ComputedStyle {
    public:

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
		FloatSize topLeft() const { return FloatSize(); }
		FloatSize topRight() const { return FloatSize(); }
		FloatSize bottomLeft() const { return FloatSize(); }
		FloatSize bottomRight() const { return FloatSize(); }
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
