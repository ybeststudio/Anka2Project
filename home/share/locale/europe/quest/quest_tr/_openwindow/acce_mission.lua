quest acce_mission begin
	state start begin
		when login or levelup with pc.get_level() >= 40 begin
			set_state("information")
		end
	end
	
	state information begin
		when enter begin
			pc.setqf("time_to_wait", 0)
			pc.setqf("collect_count", 0)
		end
		
		when letter begin
			local v = find_npc_by_vnum(20406)
			if v != 0 then
				target.vid("theowahdan", v, "Theowahdan")
			end
			
			send_letter("Özel bir kumaþ ")
		end
		
		when button or info begin
			say_title("Özel bir kumaþ ")
			say("Teovahdan seni görmek istiyor. Onu ziyaret et ve")
			say("hikayesini dinle.")
			say("")
			say_reward("Mini Harita'daki yanýp sönen noktayý takip et.")
		end
		
		when theowahdan.target.click or 20084.chat."Kendi gücünü artýr" begin
			target.delete("theowahdan")
			
			say_title(mob_name(20406))
			say("")
			say("Ah, iþte buradasýn! Sýra dýþý bir þey keþfettim:")
			say("Omuz kuþaðý adýnda yeni bir ekipman.")
			say("")
			say("Omuz kuþaðý sana iki imkan saðlar:")
			say("Kombinasyon ve emiþ.")
			say("")
			say("Kombinasyon ayný derecedeki iki kuþakla")
			say("gerçekleþtirilebilir. Kombine edilen iki kuþak")
			say("daha deðerli bir kuþak meydana getirir.")
			wait()
			say_title(mob_name(20406))
			say("")
			say("Emiþ iþleminde bir silah veya zýrh üzerinde")
			say("bulunan bonuslar")
			say("çeþitli oranlarla omuz kuþaðýna aktarýlýr.")
			say("Emiþ oraný (%) kuþaðýn derecesine baðlýdýr. Bonus")
			say("iþlemi için seçilen silah ve zýrh geri")
			say("döndürülemez þekilde yok edilir.")
			say("Ben birinci derece kuþak yapabiliyorum.")
			say("Fakat 40x "..item_name(80019).."'a ihtiyacým var.")
			say("")
			say("Sadece "..mob_name(20355).." bu maddeyi üretebilir.")
			say("Git ve onunla görüþ.")
			say("")
			set_state("go_to_disciple")
		end
	end
	
	state go_to_disciple begin
		when letter begin
			local v = find_npc_by_vnum(20355)
			if v != 0 then
				target.vid("captain", v, "Captain")
			end
			
			send_letter("Kendi gücünü artýr")
		end
		
		when button or info begin
			say_title("Kendi gücünü artýr")
			say("")
			say(""..mob_name(20355).."'yý bul ve malzemeyi al.")
			say("")
		end
		
    	when captain.target.click or 20355.chat."Kendi gücünü artýr" begin
			target.delete("captain")
			
			say_title(mob_name(20355))
			say("")
			say("Tebrikler! "..mob_name(20406).." bana ziyarete geleceðini")
			say("söyledi. Son zamanlarda Sung Mahi'nin daha da")
			say("güçlendiðini öðrendim.")
			say("En azýndan birimiz onun gücüne sahip olmalýyýz.")
			say("Yoksa Sung Mahi hepimizi yok edecek.")
			say("")
			say("Özel kumaþý üretebilirim ama bu o okadar da kolay")
			say("olmayacak.")
			say("Bana Üç saat ver. O sürede sana kumaþý ")
			say("üretebilirim.")
			say("")
			wait()
			say_title(mob_name(20355))
			say("")
			say("Üç saat sonra geldiðinde malzeme hazýr olacak.")
			say("Söz veriyorum!")
				if pc.is_gm() then
				say_reward("GM'ler hemen alýr.")
				end
			say("")
			local agree = select("Kabul et", "Reddet")
			if agree == 1 then
				if not pc.is_gm() then
					pc.setqf("time_to_wait", 60 * 3 + pc.get_playtime())
				end
				set_state("time_to_wait")
			end
		end
	end
	
	state time_to_wait begin
		when letter begin
			local v = find_npc_by_vnum(20355)
			if v != 0 then
				target.vid("captain", v, "Captain")
			end
			
			send_letter("Kendi gücünü artýr")
		end
		
		when button or info begin
			say_title("Kendi gücünü artýr")
			say("")
			say("Malzemeyi almak için "..mob_name(20355).."'yý bul.")
			say("")
			if pc.getqf("time_to_wait") > pc.get_playtime() then
				local time_to_wait = pc.getqf("time_to_wait") - pc.get_playtime()
				say("Kalan zaman: "..time_to_wait.." dakika.")
				say("")
			end
		end
		
    	when captain.target.click or 20355.chat."Kendi gücünü artýr" begin
			target.delete("captain")
			
			if pc.getqf("time_to_wait") > pc.get_playtime() and not pc.is_gm() then
				local time_to_wait = pc.getqf("time_to_wait") - pc.get_playtime()
				say("Henüz Üç saat dolmadý.")
				say("")
				say("Kalan zaman: "..time_to_wait.." dakika.")
				say("")
			else
				pc.setqf("time_to_wait", 0)
				say("Döndün demek. Ben de sözümü tuttum:")
				say("Ýþte istediðin kumaþ.")
				say("Acilen güçlerimizi artýrmaya devam etmemiz")
				say("gerektiðini unutma. Bu biraz vakit alýr.")
				say("Eðer yardýma ihtiyaç duyarsan yine yanýma gel.")
				say_item_vnum(80019)
				if pc.count_item("80019") >= 39 then
					--pc.setqf("collect_count", pc.getqf("collect_count") + 1)
					set_state("__complete")
				else
					--pc.setqf("collect_count", pc.getqf("collect_count") + 1)
					set_state("go_to_disciple")
				end
				
				pc.give_item2(80019, 1)
			end
		end
	end
	
	state __complete begin
		when enter begin
			notice("Yeterli miktarda ince kumaþ topladýn. Teovahdan'a")
			notice("geri dön ve ona kumaþlarý teslim et.")
			pc.delqf("time_to_wait")
			pc.delqf("collect_count")
		end
		
		when letter begin
			local v = find_npc_by_vnum(20406)
			if v != 0 then
				target.vid("theowahdan_reward", v, "Theowahdan")
			end
			
			send_letter("Özel bir kumaþ ")
		end
		
		when button or info begin
			say_title("Özel bir kumaþ ")
			say("")
			say("Teovahdan'ý bul ve onunla konuþ.")
			say("")
			say_reward("Mini Harita'daki yanýp sönen noktayý takip et.")
		end
		
		when theowahdan_reward.target.click or 20084.chat."Özel bir kumaþ " begin
			target.delete("theowahdan_reward")
			if pc.count_item(80019) < 40 then
				say("Ne? Yeterli malzeme yok!")
				say("")
				say("Tekrar Yüzbaþý'yla konuþ!")
				say("")
				set_state("go_to_disciple")
			else
			say("Selam. Görüyorum ki ihtiyaç duyduðumuz bütün")
			say("kumaþlarý toplamýþsýn. Artýk Sung Mahi'ye karþý ")
			say("savaþma gücüne sahip olduðun için çok mutluyum.")
			say("Tekrar görüþeceðiz!")
			say_item_vnum(85005)
				pc.remove_item(80019, 40)
				pc.give_item2_select(85005, 1)
				item.set_socket(1, 1)
				set_state("information")
			end
		end
	end
end