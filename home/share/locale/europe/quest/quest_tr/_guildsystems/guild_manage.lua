quest sample__guild_make begin
    state start begin
		when guild_man1.chat."Yeni Lonca kur" or guild_man2.chat."Yeni Lonca kur" or guild_man3.chat."Yeni Lonca kur" with not pc.hasguild() and not pc.isguildmaster() begin
			say_title("Köy Gardiyaný: ")
			say("  ")
			say(" Yeni lonca kurmak istiyor musun? ")
			say("  ")
			say(" Yeni lonca kurmak için ")
			say(" en az 55.seviyede olmalýsýn. Ayrýca ")
			say("  10.000.000 Yang gerekiyor. ")
			say("  ")
			say(" Lonca kurmak istiyor musun? ")
			say("  ")
			if select("Evet", "Hayýr ")==2 then return end
			-- let's start!
			say("  ")
			say(" Lonca ismini yaz. ")
			say("  ")
			say("  ")
			local guild_name = string.gsub(input(), "[^A-Za-z0-9 ]", " ") -- it also clean non alphanumeric characters
			local guild_len_name = string.len(guild_name)
			if not ((2 < guild_len_name) and (guild_len_name < 12)) then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Lonca ismi 3-11 karakter arasýnda olmalýdýr. ")
				say(" Lütfen tekrar deneyin. ")
				say("  ")
				say("  ")
				return
			end
			say(" Lonca ismi: "..guild_name)
			say(" Bu isimle lonca kurmak istediðine emin misin? ")
			if select("Evet", "Hayýr ")==2 then return end
			-- checks begin
			if not (pc.get_gold() >= 10000000 ) then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Yeterli yang yok! ")
				say("  ")
				say("  ")
				return
			end
			if not (pc.get_level() >= 55 ) then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Seviyen lonca kurmak için ")
				say(" yeterli deðil. ")
				say("  ")
				say("  ")
				return
			end
			if (pc.hasguild() or pc.isguildmaster()) then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Zaten bir loncaya üyesin! ")
				say("  ")
				say("  ")
				return
			end
			-- checks end
			-- so many ifs, we can simplify this by using a table
			local ret = pc.make_guild0(guild_name)
			if ret==-2 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Lonca ismi geçersiz! ")
				say("  ")
				say("  ")
			elseif ret==-1 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Lonca ismi zaten kullanýlýyor! ")
				say("  ")
				say("  ")
			elseif ret==0 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Lonca kurulamadý! ")
				say("  ")
				say("  ")
			elseif ret==1 then
				pc.change_gold(- 10000000 )
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Lonca baþarýyla kuruldu! ")
				say("  ")
				say("  ")
			elseif ret==2 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Zaten bir loncaya üyesin! ")
				say("  ")
				say("  ")
			elseif ret==3 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Zaten bir loncaya üyesin! ")
				say("  ")
				say("  ")
			end
		end
		
		when guild_man1.chat."Loncadan çýk" or guild_man2.chat."Loncadan çýk" or guild_man3.chat."Loncadan çýk" with pc.hasguild() and not pc.isguildmaster() and (pc.is_gm() or npc.empire == pc.empire) begin
			say_title("Köy Gardiyaný: ")
			say("  ")
			say(" Olduðun loncadan çýkmak mý istiyorsun? ")
			say(" Herhalde orada arkadaþ bulamadýn. ")
			say(" Nasýl istersen. ")
			say(" Loncadan çýkmak istediðine emin misin? ")
			say("  ")
			say("  ")
			local s = select("Evet", "Hayýr ")
			if s==1 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Tamam. ")
				say(" Artýk loncada deðilsin. ")
				say("  ")
				say("  ")
				pc.remove_from_guild()
				pc.setqf("new_withdraw_time",get_global_time())
			end
		end

		when guild_man1.chat."Loncayý kapat" or guild_man2.chat."Loncayý kapat" or guild_man3.chat."Loncayý kapat" with pc.hasguild() and pc.isguildmaster() and (pc.is_gm() or npc.empire == pc.empire) begin
			say_title("Köy Gardiyaný: ")
			say("  ")
			say(" Ne? ")
			say(" O kadar zahmetle bu loncayý ")
			say(" kurdum ve sen onu þimdi kapatmak mý istiyorsun? ")
			say(" Bütün hatýralarýn ve dostlarýn ")
			say(" sonsuza kadar kaybolacak! ")
			say("  ")
			say(" Loncayý kapatmak istediðine emin misin? ")
			say("  ")
			local s = select("Evet", "Hayýr ")
			if s==1 then
				say_title("Köy Gardiyaný: ")
				say("  ")
				say(" Tamam. ")
				say(" Loncayý kapattým. ")
				say("  ")
				say("  ")
				pc.destroy_guild()
				pc.setqf("new_disband_time", get_global_time())
				pc.setqf("new_withdraw_time", get_global_time())
			end
		end
	end
end
