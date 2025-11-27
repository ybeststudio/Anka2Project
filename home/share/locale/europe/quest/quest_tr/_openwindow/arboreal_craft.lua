quest arboreal_craft begin
	state start begin
		when 20015.chat."Arýndýrma " begin
			setskin(NOWINDOW)
			command("cube open")
		end
		when 20016.chat."Arýndýrma " begin
			setskin(NOWINDOW)
			command("cube open")
		end
		when 20022.chat."Arýndýrma " begin
			setskin(NOWINDOW)
			command("cube open")
		end
		when 20383.chat."Arýndýrma " begin
			setskin(NOWINDOW)
			command("cube open")
		end
		when 20349.chat."Market " begin
			npc.open_shop(5)
			setskin(NOWINDOW)
		end
		when 20406.chat."Market " begin
			npc.open_shop(11)
			setskin(NOWINDOW)
		end
	end
end