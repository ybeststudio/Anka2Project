quest guild_building_melt begin
    state start begin
        function GetOreRefineCost(cost)
            if pc.empire != npc.empire then
                return 3 * cost
            end
            if pc.get_guild() == npc.get_guild() then
                return cost * 0.9
            end
            return cost
        end

        function GetOreRefineGoodPct()
            return 60
        end

        function GetOreRefineBadPct()
            return 30
        end

        function GetMyRefineNum(race)
            return race - 20060 + 50601
        end

        function IsRefinableRawOre(vnum)
            return vnum >= 50601 and vnum <= 50613
        end

        function DoRefineDiamond(pct)
            local from_postfix
            local from_name = item_name(item.vnum)
            local to_vnum = item.vnum + 20
            local to_name = item_name(to_vnum)
            local to_postfix

            if under_han(from_name) then
                from_postfix = ""
            else
                from_postfix = ""
            end

            if under_han(to_name) then
                to_postfix = ""
            else
                to_postfix = ""
            end

            say("100 " .. from_name .. from_postfix .. " toplaman lazým ki ")
            say(to_name .. to_postfix .. "üretimi yapabilesin.")
            if item.count >= 100 then
                say("Baþarý ihtimalin "..pct.."%.")
				say(""..guild_building_melt.GetOreRefineCost(10000).." Yang gerekli.")
                say("Denemek istiyormusun?")
                local s = select("Evet", "Hayýr")
                if s == 1 then
                    if pc.get_gold() < guild_building_melt.GetOreRefineCost(10000) then
                        say("Yeterli yang yok.")
                        return
                    end

                    if pc.diamond_refine(10000, pct) then
                        say("Tebrikler! Cevherler baþarýyla iþlendi. þimdi senin:")
                        say_item(to_name, to_vnum, "")
                    else
                        say("Cevher iþleme baþarýsýz oldu.")
                    end
                end
            else
                say("100 tane topla...")
            end
        end
        function DoRefine(pct)
            local from_postfix
            local from_name = item_name(item.vnum)
            local to_vnum = item.vnum + 20
            local to_name = item_name(to_vnum)
            local to_postfix

            if under_han(from_name) then
                from_postfix = ""
            else
                from_postfix = ""
            end

            if under_han(to_name) then
                to_postfix = ""
            else
                to_postfix = ""
            end

            say("Önce 100 " .. from_name .. from_postfix .. "toplaman lazým ki")
            say("Ruh Taþý +0, Ruh Taþý +1 veya Ruh Taþý +2")
            say(to_name .. to_postfix .. "üretebilesin.")
            if item.count >= 100 then
                say("Baþarý ihtimalin"..pct.."%."..guild_building_melt.GetOreRefineCost(3000).." Yang ödemen gerekiyor.")
                say("Denemek istiyor musun?")
                local s = select("Evet", "Hayýr")
                if s == 1 then
                    if pc.get_gold() < guild_building_melt.GetOreRefineCost(3000) then
                        say("Yeterli Yang yok.")
                        return
                    end

                    local selected_item_cell = select_item()
                    if selected_item_cell == 0 then
                        say("Bana meteoridleri göster.")
                        return
                    end
                    local old_item = item.get_id()

                    if not item.select_cell(selected_item_cell) then
                        say("Bu istediðimiz malzeme deðil.")
                        return
                    end

                    -- checken, ob der meteorit ist
                    if item.vnum < 28000 or item.vnum >= 28300 then
                        say("Bu doðru madde deðil.")
                        return
                    end

                    item.select(old_item)

                    if pc.ore_refine(3000, pct, selected_item_cell) then
                        say("Tebrik ederim, islem tamamlandi.Sonuç:")
                        say_item(to_name, to_vnum, "")
                    else
                        say("Ýþlem baþarýlý olmadý.")
                    end
                end
            else
                say("100 tane topla.")
            end
        end

        when
            20060.take or
            20061.take or
            20062.take or
            20063.take or
            20064.take or
            20065.take or
            20066.take or
            20067.take or
            20068.take or
            20069.take or
            20070.take or
            20071.take or
            20072.take
            with guild_building_melt.GetMyRefineNum(npc.race) == item.vnum
        begin
            if item.vnum == 50601 then
                guild_building_melt.DoRefineDiamond(guild_building_melt.GetOreRefineGoodPct())
            else
                guild_building_melt.DoRefine(guild_building_melt.GetOreRefineGoodPct())
            end
        end

        when
            20060.take or
            20061.take or
            20062.take or
            20063.take or
            20064.take or
            20065.take or
            20066.take or
            20067.take or
            20068.take or
            20069.take or
            20070.take or
            20071.take or
            20072.take
            with guild_building_melt.IsRefinableRawOre(item.vnum) and
guild_building_melt.GetMyRefineNum(npc.race) != item.vnum
        begin
            if item.vnum == 50601 then
                guild_building_melt.DoRefineDiamond(guild_building_melt.GetOreRefineBadPct())
            else
                guild_building_melt.DoRefine(guild_building_melt.GetOreRefineBadPct())
            end
        end

        when
            20060.click or
            20061.click or
            20062.click or
            20063.click or
            20064.click or
            20065.click or
            20066.click or
            20067.click or
            20068.click or
            20069.click or
            20070.click or
            20071.click or
            20072.click
            with npc.get_guild() == pc.get_guild() and pc.isguildmaster()
        begin
            say(" 3.000.000 Yang karþýlýðýnda iyi bir kimyager")
			say("kullanabilirsin.")
            if pc.get_gold() < 3000000 then
                say("3.000.000 Yang getir.")
            else
                say("Hangi tür üzerinde çalýþan kimyager istiyorsun?")

                local sel = 0
                local timetable1 = {"Elmas", "Fosiller", "Bakýr", "Gümüþ", "Altýn", "Yeþim", "Devam", "Kapat"}
                local valuetable1 = {14043, 14045, 14046, 14047, 14048, 14049, 0, -1}
                local timetable2 = {"Abanoz", "Ýnci", "Beyaz Altýn", "Kristal", "Ametist", "Elmas", "Devam", "Kapat"}
                local valuetable2 = {14050, 10451, 14052, 14053, 14054, 14055, 0, -1}
                repeat
                        local s = select_table(timetable1)
                        sel = valuetable1[s]
                        if sel == 0 then
                                local s = select_table(timetable2)
                                sel = valuetable2[s]
                        end
                until sel != 0
                if sel != -1 then
                    npc_num = sel + 20060 - 14043
                    if npc_num == npc.get_race() then
                        say("Bu malzeme ile çalýsmasý mümkün deðil.")
                    else
                        pc.changegold(-3000000)
                        building.reconstruct(sel)
                    end
                else
                    say("Kapat")
                end
            end
        end
    end
end
