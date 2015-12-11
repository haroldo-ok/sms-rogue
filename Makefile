SHELL=C:/Windows/System32/cmd.exe
CC	= sdcc -mz80

BINS	= sms-rogue.sms


all:	$(BINS)

gfx.c: gfx/font.fnt
	folder2c gfx gfx

sms-rogue.rel: gfx.c

%.rel:	%.c
	$(CC) -c --std-sdcc99 $< -o $@

%.sms:	%.ihx
	ihx2sms $< $@

sms-rogue.ihx:	sms-rogue.rel SMSlib/SMSlib.rel gfx.rel map.rel actor.rel
	$(CC) --data-loc 0xC000 $^

clean:
	rm -f *.ihx *.lk *.lst *.map *.noi *.rel *.sym *.asm *.sms gfx.*
