quest mount_up_grade begin
	state start begin
		when 20349.chat."At seviyesi artırıldı " begin
			setskin(NOWINDOW)
			game.open_mount_up_grade()
		end
	end
end
