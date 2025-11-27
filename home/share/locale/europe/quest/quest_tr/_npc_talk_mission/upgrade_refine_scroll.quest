quest upgrade_refine_scroll begin
	state start begin
		when 20016.chat."Savaþ kaðýdý üret." begin
            say_title("Demirci:")
            say("Savaþ Tanrýlarý tarafýndan kutsanmýþ gizemli bir")
            say("kaðýt var, bu kaðýt eþyalarýn +0'dan +3'e")
            say("kadar yükseltmesinin baþarý oranýný %100'e")
            say("çýkarýyor. Ancak bu kaðýt +4 ve üstü için")
            say("çalýþmýyor.")
            say("")
            wait()
            if pc.count_item(70035) == 0 then
                say_title("Demirci:")
                say("Bunu üretmek için, büyülü metale ihtiyacýn")
                say("var.")
                say("")
                say_item("Büyülü metal", 70035, "")
                say("")
                wait()
				end
            if pc.count_item(25040) == 0 then
                say_title("Demirci:")
                say("Ayrýca kutsama kaðýdýna da ihtiyacýn ")
                say("var.")
                say("")
                say_item("Kutsama kaðýdý ", 25040, "")
                say("")
                wait()
            end
            say_title("Demirci:")
            say("Tüm gereken eþyalara sahip misin?")
            say("Bir tane üretmek istiyor musun?")
            say("Yanýsýra 200.000 Yang'a ihtiyacýn olacak.")
            say("Eðer büyülü metal ve kutsama kaðýdýn varsa,")
            say("savaþ kaðýdý üretebilirsin. Bu kaðýdý üretmek")
            say("istiyor musun?")
            say("")
            local s = select("Evet", "Hayýr")
            if 1 == s then
                if pc.get_gold() < 200000 then
                    say_title("Bilgi:")
                    say_reward("Yeterli Yang'ýn yok.")
                    say("")
                elseif pc.count_item(70035)>0 and pc.count_item(25040)>0 then
                    pc.remove_item(70035)
                    pc.remove_item(25040)
                    pc.changegold(-200000)
                    pc.give_item2(71021)
                    say_title("Demirci:")
                    say("Bir savaþ kaðýdý ürettin!")
                    say("Büyülü metalin olduðunda savaþ ")
                    say("kaðýdý üretmek gerçekten kolay.")
                    say("")
                    say_item("Savaþ kaðýdý ", 71021,"")
                else
                    say_title("Demirci:")
					say("Ýhtiyacýn olan tüm nesnelere")
                    say("sahip deðilsin.")
                    say("")
                end
            end
        end
        when 20016.chat."Ejderha Tanrýsý Kaðýdý üretmek" begin
            say_title("Demirci:")
            say("Ejderha Tanrýsý kaðýdý geliþtirmelerinde baþarý ")
            say("yüzdeni artýrýr. Eðer baþarýsýz olursan, sadece")
            say("1 seviye düþer. Bu gizemli eþya bir tür kutsama")
            say("kaðýdý olarak görülebilir.")
            say("")
            wait()
            if pc.count_item(71026)==0 then
                say_title("Demirci:")
                say("")
                say("Bunu yapmak için büyülü metale")
                say("ihtiyacýn var.")
				say("")
                say_item("Büyülü Metal", 71026, "")
                say("")
                wait()
            end
            if pc.count_item(25040)==0 then
                say_title("Demirci:")
                say("Bunu üretebilmen için, kutsama kaðýdý gerekli.")
                say("")
                say_item("Kutsama kaðýdý ", 25040,"")
                say("")
                wait()
            end
            say_title("Demirci:")
            say("Tüm gereken eþyalara sahip misin? Bunlardan")
            say("birini üretmek istiyor musun? Eðer kutsama kaðýdý ")
            say("ve büyülü demir madeni kullanýrsan, Ejderha")
            say("Tanrýsý kaðýdý yapabilirsin.")
            say("")
            local ss = select("Evet", "Hayýr")
            if 1 == ss then
                if pc.count_item(71026)>0 and pc.count_item(25040)>0 then
                    pc.remove_item(71026)
                    pc.remove_item(25040)
                    pc.give_item2(71032)
                    say_title("Demirci:")
                    say("Bir Ejderha Tanrýsý kaðýdý ürettin! ")
                    say("Büyülü metal kullandýðýn zaman,")
                    say("çok kolay bir Ejderha Tanrýsý kaðýdý ")
                    say("yaparsýn.")
                    say("")
                    say_item("Ejderha Tanrýsý kaðýdý ", 71032, "")
                else
                    say_title("Demirci:")
                    say("Ejderha Tanrýsý kaðýdý üretme iþlemi baþarýsýz")
                    say("oldu, çünkü gereken tüm eþyalara sahip deðilsin. ")
                    say("")
                end
            end
        end

        when 20016.chat."Demircinin El Kitabý üret" begin
                say_title("Demirci:")
                say("Hiç Demirci Taþý diye bir þey duymuþ muydun? Bu")
                say("taþ demircinin kaný ve alýn terinden yapýlýr.")
                say("Eðer bana demircinin taþýný ve Kutsama Kaðýdý ")
                say("getirirsen sana ailemin geçmiþinden bugüne gelmiþ ")
                say("olan bir el kitabý yaparým. Bu el kitabý, ejderha")
                say("tanrýsý kaðýdý ile ayný iþe yarar fakat baþarý ")
                say("yüzdesi daha yüksektir.")
                say("")
                wait()
                if pc.count_item(71025) == 0 then
                        say_title("Demirci:")

                        say("Bundan üretebilmen için, Demircinin taþýna")
                        say("ihtiyacin var.")
                        say("")
                        say_item("Demircinin Taþý ", 71025, "")
                        say("")
                        wait()
                end
                if pc.count_item(25040) == 0 then
                        say_title("Demirci:")
                        say("Bunu üretebilmek için kutsama kaðýdýna")
                        say("ihtiyaç var.")
                        say("")
                        say_item("Kutsama kaðýdý ", 25040, "")
                        say("")
                        wait()
                end
                say_title("Demirci:")
                say("")
                say("Gereken tüm eþyalara sahip misin? Peki,")
                say("demircinin el kitabýný yapmak istiyor musun?")
                say("")
                local sss = select("Evet", "Hayýr")
                if 1 == sss then
                        if pc.count_item(71025) > 0 and pc.count_item(25040) > 0 then
                                pc.remove_item(71025)
                                pc.remove_item(25040)
                                pc.give_item2(70039)
                                say_title("Demirci:")
                                say("Baþarýyla bir demircilik el kitabý ")
                                say("yaptýn! Ailemin El Kitabýný kaðýda ")
                                say("yazacaðým. ")
                                say("")
                                say_item("Demircinin El Kitabý ", 70039, "")
                        else
                                say_title("Demirci:")
                                say("Demircinin el kitabýný üretemiyorum çünkü gereken")
                                say("malzemelere sahip deðilsin.")
                                say("")
                        end
                end
        end
    end
end
