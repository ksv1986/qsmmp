# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $ 

EAPI=3
inherit qt4-r2 git-2

EGIT_REPO_URI="git://gitorious.org/qsmmp/${PN}.git"
EGIT_BRANCH="qmmp-9999"
EGIT_COMMIT="${EGIT_BRANCH}"

DESCRIPTION="QSMMP is alternative UI for QMMP implemented as a plugin with
native Qt look."
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="=media-sound/qmmp-9999
		x11-libs/libqxt"

DEPEND="${RDEPEND}"

