quest adminsystem begin
	state start begin
		when letter with pc.is_gm() begin
			send_letter ("GM: Yönetim Paneli ")
		end
		when button or info begin
		say_title ("Yönetim Paneli ")
		say ("")
		local head = select("Admin Ban", "Admin Mesaj", "Kapat")
			if head == 1 then
				cmdchat("OpenAdminTool")
				setskin(NOWINDOW)
			
			elseif head == 2 then
				cmdchat("OpenWhisperTool")
				setskin(NOWINDOW)
			end
		end
	end
end
