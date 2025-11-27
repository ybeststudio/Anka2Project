quest theovahdan_2 begin
	state start begin
		when 20406.chat."Omuz kuþaðý nedir? " begin
			say_title(mob_name(20406))
			say("")
			say("Kombinasyon ayný derecedeki iki kuþakla")
			say("gerçekleþtirilebilir. Kombine edilen iki kuþak")
			say("daha deðerli bir kuþak meydana getirir.")
			say("")
			say("Emiþ iþleminde bir silah veya zýrh üzerinde")
			say("bulunan bonuslar")
			say("çeþitli oranlarla omuz kuþaðýna aktarýlýr.")
			say("")
			say("Emiþ oraný (%) kuþaðýn derecesine baðlýdýr. Bonus")
			say("iþlemi için seçilen silah ve zýrh geri")
			say("döndürülemez þekilde yok edilir.")
			say("")
		end
		when 20406.chat."Kombinasyon " begin
			say_title(mob_name(20406))
			say("")
			say("Ýki kuþaðý kombine mi etmek istiyorsun?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			pc.open_acce(true)
		end
		when 20406.chat."Bonus Emiþi " begin
			say_title(mob_name(20406))
			say("")
			say("Silah veya zýrhýndan bonus mu emmek istersin?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			pc.open_acce(false)
		end
		when 20406.chat."Bonuslarý Aktar " begin
			say_title(mob_name(20406))
			say("")
			say("2. kostümündeki efsunlarý 1. kostümüne")
			say("aktarabilirsin.")
			say("")
			say("Bu iþlemi yapabilmek için önce")
			say("transfer nesnesini pencereye eklemelisin.")
			say("")
			say("2. kostüm yok edilecektir..")
			say("")
			say("Devam etmek istiyor musun ?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			pc.open_acce(false)
		end
		when 20406.chat."Yansýtma " begin
			say_title(mob_name(20406))
			say("")
			say("Yansýtma, ekipmanýnýn görünümü sonsuza dek")
			say("deðiþtirmeni saðlar.Ancak etkisi ekipmanýn")
			say("seviyesiyle sýnýrlýdýr ve eklediðin yansýtma")
			say("nesne geliþtirildiðinde yok olur.Devam et?")
			say("")
			local confirm = select("Evet.", "Hayýr")
			if confirm == 2 then
				return
			end
			setskin(NOWINDOW)
			game.open_transmutation(false)
		end
	end
end