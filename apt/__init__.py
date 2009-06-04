#  Copyright (c) 2005-2009 Canonical
#
#  Author: Michael Vogt <michael.vogt@ubuntu.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA
# import the core of apt_pkg
"""High-Level Interface for working with apt."""
import apt_pkg

# import some fancy classes
from apt.package import Package
from apt.cache import Cache
from apt.cdrom import Cdrom

if apt_pkg._COMPAT_0_7:
    from apt.progress import (OpProgress, FetchProgress, InstallProgress,
                              CdromProgress)


if apt_pkg._COMPAT_0_7:
    from apt_pkg import (size_to_str as SizeToStr,
                         time_to_str as TimeToStr,
                         version_compare as VersionCompare)

# init the package system
apt_pkg.init()


#import warnings
#warnings.warn("apt API not stable yet", FutureWarning)
#del warnings
__all__ = ['Cache', 'Cdrom', 'Package']
