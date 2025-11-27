------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------

quest guild_ranking begin
    state start begin
        when 11000.chat."Kendi loncanýn sýralamamý göster" or
             11002.chat."Kendi loncanýn sýralamamý göster" or
             11004.chat."Kendi loncanýn sýralamamý göster" with pc.hasguild() begin
            setskin(NOWINDOW)
            pc.open_guild_ranking()
        end
    end
end