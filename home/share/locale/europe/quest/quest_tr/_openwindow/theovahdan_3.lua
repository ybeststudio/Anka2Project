quest theovahdan_4 begin
	state start begin
		when 20406.chat."Aura Giysisi: Deðerleri Aktar" begin
			say_title(mob_name(20406))
			say("")
			say("Aura giysine bileklik,kolye,küpe ve kalkan")
			say("deðerlerini aktarmak istiyor musun?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			game.open_aura_absorb_window()
		end
		when 20406.chat."Aura Giysisi: Yükseltme" begin
			say_title(mob_name(20406))
			say("")
			say("Aura giysisini geliþtirmek istiyor musun?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			game.open_aura_growth_window()
		end
		when 20406.chat."Aura Giysisi: Geliþim" begin
			say_title(mob_name(20406))
			say("")
			say("Aura giysisini geliþtirmek istiyor musun?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			game.open_aura_evolve_window()
		end
	end
end