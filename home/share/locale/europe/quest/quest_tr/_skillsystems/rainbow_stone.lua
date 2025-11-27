------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest rainbow_stone begin
	state start begin
		when 50512.use with pc.is_gm() begin
			say_title("Gökkusagi tasi")
			say_new("[ENTER]Bu tas tüm yetenekleri mükemmel yapar.")

			if pc.job ==0 then
				say_new("Seçin: [ENTER]")

				local s = select("Bedensel Savas Egitimi", "Zihinsel Savas Egitimi")
				if s == 1 then
					pc.set_skill_level(1 ,59)
					pc.set_skill_level(2 ,59)
					pc.set_skill_level(3 ,59)
					pc.set_skill_level(4 ,59)
					pc.set_skill_level(5 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

				if s == 2 then
					pc.set_skill_level(16 ,59)
					pc.set_skill_level(17 ,59)
					pc.set_skill_level(18 ,59)
					pc.set_skill_level(19 ,59)
					pc.set_skill_level(20 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

			elseif pc.job == 1 then
				say_new("Seçin: [ENTER]")

				local s = select("Yakin Dövüs Egitimi", "Uzak Dövüs Egitimi")
				if s == 1 then
					pc.set_skill_level(31 ,59)
					pc.set_skill_level(32 ,59)
					pc.set_skill_level(33 ,59)
					pc.set_skill_level(34 ,59)
					pc.set_skill_level(35 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

				if s == 2 then
					pc.set_skill_level(46 ,59)
					pc.set_skill_level(47 ,59)
					pc.set_skill_level(48 ,59)
					pc.set_skill_level(49 ,59)
					pc.set_skill_level(50 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

			elseif pc.job == 2 then
				say_new("Seçin: [ENTER]")

				local s = select("Büyülü Silah Egitimi", "Kara Büyü Egitimi")
				if s == 1 then
					pc.set_skill_level(76 ,59)
					pc.set_skill_level(77 ,59)
					pc.set_skill_level(78 ,59)
					pc.set_skill_level(79 ,59)
					pc.set_skill_level(80 ,59)
					pc.set_skill_level(81, 59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

				if s == 2 then
					pc.set_skill_level(61 ,59)
					pc.set_skill_level(62 ,59)
					pc.set_skill_level(63 ,59)
					pc.set_skill_level(64 ,59)
					pc.set_skill_level(65 ,59)
					pc.set_skill_level(66 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

			elseif pc.job == 3 then
				say_new("Seçin: [ENTER]")

				local s = select("Iyilestirme Egitimi", "Ejderha Gücü Egitimi")
				if s == 1 then
					pc.set_skill_level(106 ,59)
					pc.set_skill_level(107 ,59)
					pc.set_skill_level(108 ,59)
					pc.set_skill_level(109 ,59)
					pc.set_skill_level(110 ,59)
					pc.set_skill_level(111 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end

				if s == 2 then
					pc.set_skill_level(91 ,59)
					pc.set_skill_level(92 ,59)
					pc.set_skill_level(93 ,59)
					pc.set_skill_level(94 ,59)
					pc.set_skill_level(95 ,59)
					pc.set_skill_level(96 ,59)

					say_title("Gökkusagi tasi")
					say_new("[ENTER]Tüm yetenekleriniz zaten mükemmel. [ENTER]")
					pc.remove_item(50512, 1)
				end
			end
		end
	end
end