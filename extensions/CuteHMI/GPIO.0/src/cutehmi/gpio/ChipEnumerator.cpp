#include <cutehmi/gpio/ChipEnumerator.hpp>

#include <gpiod.h>

template class cutehmi::Singleton<cutehmi::gpio::ChipEnumerator>;

namespace cutehmi {
namespace gpio {

void ChipEnumerator::update()
{
	::gpiod_chip_iter * iter = ::gpiod_chip_iter_new();
	::gpiod_chip * chip = gpiod_chip_iter_next(iter);
	while (chip != nullptr) {
		CUTEHMI_WARNING("Name: " << ::gpiod_chip_name(chip));
		CUTEHMI_WARNING("Label: " << ::gpiod_chip_label(chip));
		CUTEHMI_WARNING("Num lines: " << ::gpiod_chip_num_lines(chip));
		chip = gpiod_chip_iter_next(iter);
	}
	::gpiod_chip_iter_free(iter);
}

ChipEnumerator::ChipEnumerator()
{
}

}
}

//(c)MP: Copyright © 2019, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
