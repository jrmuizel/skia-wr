#include "LayoutRectOutsets.h"
namespace blink {
class ComputedStyle {
        public:

		FloatRoundedRect getRoundedInnerBorderFor(const LayoutRect& borderRect,
							  const LayoutRectOutsets insets, bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const {
    LayoutRect innerRect(borderRect);
    innerRect.expand(insets);

    FloatRoundedRect roundedRect(pixelSnappedIntRect(innerRect));
	return roundedRect;


}
};
	bool m_hasBorderRadius;
}
