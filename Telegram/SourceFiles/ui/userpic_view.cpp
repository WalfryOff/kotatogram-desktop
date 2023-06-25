/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "ui/userpic_view.h"

#include "ui/empty_userpic.h"
#include "ui/image/image_prepare.h"

namespace Ui {

float64 ForumUserpicRadiusMultiplier() {
	return 0.3;
}

bool PeerUserpicLoading(const PeerUserpicView &view) {
	return view.cloud && view.cloud->isNull();
}

void ValidateUserpicCache(
		PeerUserpicView &view,
		const QImage *cloud,
		const EmptyUserpic *empty,
		int size,
		bool forum,
		float64 radiusMultiplier) {
	Expects(cloud != nullptr || empty != nullptr);

	const auto full = QSize(size, size);
	const auto version = style::PaletteVersion();
	const auto forumValue = forum ? 1 : 0;
	const auto regenerate = (view.cached.size() != QSize(size, size))
		|| (view.forum != forumValue)
		|| (cloud && !view.empty.null())
		|| (empty && empty != view.empty.get())
		|| (empty && view.paletteVersion != version);
	if (!regenerate) {
		return;
	}
	view.empty = empty;
	view.forum = forumValue;
	view.radiusMultiplier = radiusMultiplier;
	view.paletteVersion = version;

	if (cloud) {
		view.cached = cloud->scaled(
			full,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);
		if (view.radiusMultiplier < 0.0) {
			view.cached = Images::Circle(std::move(view.cached));
		} else if (view.radiusMultiplier > 0.0) {
			view.cached = Images::Round(
				std::move(view.cached),
				Images::CornersMask(size
					* view.radiusMultiplier
					/ style::DevicePixelRatio()));
		}
	} else {
		if (view.cached.size() != full) {
			view.cached = QImage(full, QImage::Format_ARGB32_Premultiplied);
		}
		view.cached.fill(Qt::transparent);

		auto p = QPainter(&view.cached);
		if (view.radiusMultiplier < 0.0) {
			empty->paintCircle(p, 0, 0, size, size);
		} else if (view.radiusMultiplier > 0.0) {
			empty->paintRounded(
				p,
				0,
				0,
				size,
				size,
				size * view.radiusMultiplier);
		} else {
			empty->paintSquare(p, 0, 0, size, size);
		}
	}
}

} // namespace Ui
