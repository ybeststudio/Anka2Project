quest theovahdan_3 begin
	state start begin
		when 20406.chat."Binek Dönüþümü " begin
			setskin(NOWINDOW)
			game.open_transmutation(true)
		end
	end
end