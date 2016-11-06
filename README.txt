Name: Kelton Hislop

Instructions and Notes:
Everything should work as intended, just make and run.

My program has a simple check sum that takes each 8 bit character and adds it together. The should catch most corruption. A case where the check sum could succeed wouold be if some of the bytes increased in value and some decreased in value and the sum of the increase and decrese was 0. The other situation where it would fail is if the corrupted message increased the bytes to sum to exactly the amount of an int pluse the original sum. In that case, the checksum will roll over and add up to the same check sum. 
