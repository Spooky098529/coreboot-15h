/*
 * acpi/sleep.asl -- Minimal sleep transition methods
 *
 */

Method(\_PTS, 1) {
	if (LEqual(Arg0, 3)) {
	}
}

Method(\_BFS, 1) {
}

Method(\_WAK, 1) {
	if (LEqual(Arg0, 3)) {
	}
	Return(Package(){0, 0})
}
