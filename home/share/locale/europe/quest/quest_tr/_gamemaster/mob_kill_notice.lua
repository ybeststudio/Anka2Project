------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest mob_kill_notice begin
	state start begin
		when 691.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(300)
			chat("300 derece puani aldiniz!")
			notice_all(617, pc.get_name())
		end
		when 791.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(500)
			chat("500 derece puani aldiniz!")
			notice_all(618, pc.get_name())
		end
		when 792.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(900)
			chat("900 derece puani aldiniz!")
			notice_all(619, pc.get_name())
		end
		when 1304.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(620, pc.get_name())
		end
		when 1091.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(600)
			chat("600 derece puani aldiniz!")
			notice_all(621, pc.get_name())
		end
		when 1092.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(800)
			chat("800 derece puani aldiniz!")
			notice_all(622, pc.get_name())
		end
		when 1093.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(623, pc.get_name())
		end
		when 1901.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(624, pc.get_name())
		end
		when 2091.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(625, pc.get_name())
		end
		when 2092.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(626, pc.get_name())
		end
		when 2191.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(627, pc.get_name())
		end
		when 2491.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(628, pc.get_name())
		end
		when 2492.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(629, pc.get_name())
		end
		when 5002.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment( 2000)
			chat("2000 derece puani aldiniz!")
			notice_all(630, pc.get_name())
		end
		when 2206.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment( 1000)
			chat("1000 derece puani aldiniz!")
			notice_all(631, pc.get_name())
		end
		when 2306.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment( 1000)
			chat("1000 derece puani aldiniz!")
			notice_all(632, pc.get_name())
		end
		when 2307.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(1000)
			chat("1000 derece puani aldiniz!")
			notice_all(633, pc.get_name())
		end
		when 1191.kill begin
			if pc.is_gm() then
				return
			end

			pc.change_alignment(300)
			chat("300 derece puani aldiniz!")
			notice_all(634, pc.get_name())
		end
	end
end