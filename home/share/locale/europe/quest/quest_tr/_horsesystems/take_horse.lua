quest horse_summon begin
		state start begin
				function get_horse_summon_prob_pct()
						local skill_level = pc.get_skill_level(131)
						if skill_level == 1 then
								return 15
						elseif skill_level == 2 then
								return 20
						elseif skill_level == 3 then
								return 30
						elseif skill_level == 4 then
								return 40
						elseif skill_level == 5 then
								return 50
						elseif skill_level == 6 then
								return 60
						elseif skill_level == 7 then
								return 70
						elseif skill_level ==  8 then
								return 80
						elseif skill_level == 9 then
								return 90
						elseif skill_level >= 10 then
								return 100
						end

						return 10
				end

				when 50051.use with horse.get_grade() == 0 begin
					syschat("Acemi seviyedeki atýný çaðýrabilmek için acemi at görevini yapman gerek.")
				end

				when 50051.use with horse.get_grade() == 1 begin
					if horse.is_summon() == true then
						syschat("Zaten Acemi Atý çaðýrmýþsýn.")
						return
					end
					if number(1, 100) <= horse_summon.get_horse_summon_prob_pct() then
						horse.summon()
						syschat("Acemi at çaðýrýldý.")
					else
						syschat("Acemi at çaðýrýlamadý.")
					end
				end

				when 50051.use with horse.get_grade() == 2 begin
					syschat("Zýrhlý seviyedeki atýný çaðýrabilmek için Zýrhlý At Kitabý lazým.")
				end

				when 50051.use with horse.get_grade() == 3 begin
					syschat("Asker seviyedeki atýný çaðýrman için Asker At Kitabý lazým.")
				end

				when 50051.use with horse.get_grade() == 4 begin
					syschat("Güçlü Savaþ seviyedeki atýný çaðýrman için Altýn At Kitabý lazým.")
				end

				when 50052.use with horse.get_grade() == 0 begin
					syschat("Orta seviyedeki atýný çaðýrman için acemi at görevini yapman gerek.")
				end

				when 50052.use with horse.get_grade() == 1 begin
					syschat("Acemi seviyedeki atýný çaðýrman için At Resmi lazým.")
				end

				when 50052.use with horse.get_grade() == 2 begin
					if horse.is_summon() == true then
						syschat("Zaten Zýrhlý Atý çaðýrmýþsýn.")
						return
					end
					if number(1, 100) <= horse_summon.get_horse_summon_prob_pct() then
						horse.summon()
						syschat("Zýrhlý at çaðýrýldý.")
					else
						syschat("Zýrhlý at çaðýrýlamadý.")
					end
				end

				when 50052.use with horse.get_grade() == 3 begin
					syschat("Asker seviyedeki atýný çaðýrman için Asker At Kitabý lazým.")
				end

				when 50052.use with horse.get_grade() == 4 begin
					syschat("Güçlü Savaþ seviyedeki atýný çaðýrman için Altýn At Kitabý lazým.")
				end

				when 50053.use with horse.get_grade() == 0 begin
					syschat("Asker seviyedeki atýný çaðýrman için acemi at görevini yapman gerek.")
				end

				when 50053.use with horse.get_grade() == 1 begin
					syschat("Acemi seviyedeki atýný çaðýrman için At Resmi lazým.")
				end

				when 50053.use with horse.get_grade() == 2 begin
					syschat("Orta seviyedeki atýný çaðýrman için Zýrhlý At Kitabý lazým.")
				end

				when 50053.use with horse.get_grade() == 3 begin
					if horse.is_summon() == true then
						syschat("Zaten Asker Atý çaðýrmýþsýn.")
						return
					end
					if number(1, 100) <= horse_summon.get_horse_summon_prob_pct() then
						horse.summon()
						syschat("Asker at çaðýrýldý.")
					else
						syschat("Asker at çaðýrýlamadý.")
					end
				end

				when 50053.use with horse.get_grade() == 4 begin
					syschat("Güçlü Savaþ seviyedeki atýný çaðýrman için Altýn At Kitabý lazým.")
				end

				when 50049.use with horse.get_grade() == 0 begin
					syschat("Güçlü Savaþ seviyedeki atýný çaðýrman için acemi at görevini yapman gerek.")
				end

				when 50049.use with horse.get_grade() == 1 begin
					syschat("Acemi seviyedeki atýný çaðýrman için At Resmi lazým.")
				end

				when 50049.use with horse.get_grade() == 2 begin
					syschat("Orta seviyedeki atýný çaðýrman için Zýrhlý At Kitabý lazým.")
				end

				when 50049.use with horse.get_grade() == 3 begin
					syschat("Asker seviyedeki atýný çaðýrman için Asker At Kitabý lazým.")
				end

				when 50049.use with horse.get_grade() == 4 begin
					if horse.is_summon() == true then
						syschat("Zaten Güçlü Savaþ atý çaðýrmýþsýn.")
						return
					end
					if number(1, 100) <= horse_summon.get_horse_summon_prob_pct() then
						horse.summon()
						syschat("Güçlü Savaþ at çaðýrýldý.")
					else
						syschat("Güçlü Savaþ at çaðýrýlamadý.")
					end
				end

	end
		state __COMPLETE__ begin
				when enter begin
						q.done()
				end
		end
end
