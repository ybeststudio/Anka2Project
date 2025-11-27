#by [DeV]EDW4RD (Best Studio)

echo -e "\033[32m Ne yapmak istiyorsunuz? \033[0m"
echo -e "\033[32m
1 - Sunucuyu başlat \n
2 - Sunucuyu durdur \n
3 - Logları temizle \n
4 - Quest derle \n
5 - Yedek oluştur \n
6 - İptal \033[0m"
read chs

case $chs in 
1*) 
	echo -e "\033[32m"
	read -p "Kaç kanal başlatmak istiyorsunuz? (1/4) : " rch

	if [ -z $rch ] || [ $rch -lt 1 ] || [ $rch -gt 4 ] ; then
		exit
	fi

	echo -e "\033[0m"
	echo -e "\033[32m"

	SFROOT=$PWD
	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "# DB başlatılıyor..."
	cd $SFROOT/main/db/
	./vrunner --daemon --pid-path=pid --file=db >> start.log 2>&1 & sleep 2

	i1=1
	while [ $i1 -le $rch ]
	do
		for i2 in 1 2 3
		do
			echo -e "# Channel$i1 core$i2 başlatılıyor..."
			cd $SFROOT/main/channels/channel$i1/core$i2/
			./vrunner --daemon --pid-path=pid --file=game >> start.log 2>&1 & sleep 1
		done
		i1=$(( $i1 + 1 ))
	done

	echo -e "# Channel99 başlatılıyor..."
	cd $SFROOT/main/channels/channel99/
	./vrunner --daemon --pid-path=pid --file=game99 >> start.log 2>&1 & sleep 1

	echo -e "# Auth başlatılıyor..."
	cd $SFROOT/main/auth/
	./vrunner --daemon --pid-path=pid --file=auth >> start.log 2>&1 & sleep 1

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "\033[0m"

;;
2*)
	SFROOT=$PWD

	echo -e "\033[32m"

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "# Auth durduruluyor"
	cd $SFROOT/main/auth/
	if [ -r ./pid ]; then
		pkill -1 `cat ./pid`
	fi

	for i1 in 1 2 3 4
	do
		for i2 in 1 2 3
		do
			echo -e "# Channel$i1 core$i2 durduruluyor"
			cd $SFROOT/main/channels/channel$i1/core$i2/
			if [ -r ./pid ]; then
				pkill -1 `cat ./pid`
			fi
		done
	done

	echo -e "# Channel99 durduruluyor"
	cd $SFROOT/main/channels/channel99/
	if [ -r ./pid ]; then
		pkill -1 `cat ./pid`
	fi

	echo -e "# DB durduruluyor"
	cd $SFROOT/main/db/
	if [ -r ./pid ]; then
		pkill -1 `cat ./pid`
	fi

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "\033[0m"

;;
3*)
	SFROOT=$PWD

	echo -e "\033[32m"

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "# Loglar başarıyla temizlendi."

	GameFiles()
	{
		rm -rf log/*
		rm -rf start.log
		rm -rf packet_info.txt
		rm -rf syserr
		rm -rf syslog
		rm -rf stdout
		rm -rf PTS
		rm -rf p2p_packet_info.txt
		rm -rf mob_count
		rm -rf DEV_LOG.log
		rm -rf version.txt
		rm -rf udp_packet_info.txt
		rm -rf usage.txt
		rm -rf mob_data.txt
		rm -rf pid

		rm -rf game.core
		rm -rf db.core
	}
	
	SQLFiles()
	{
		rm -rf *.err
		rm -rf *.pid
	}

	cd $SFROOT/main/db/
	GameFiles

	for i1 in 1 2 3 4
	do
		for i2 in 1 2 3
		do
			cd $SFROOT/main/channels/channel$i1/core$i2/
			GameFiles
		done
	done

	cd $SFROOT/main/channels/channel99/
	GameFiles

	cd $SFROOT/main/auth/
	GameFiles
	
	cd /var/db/mysql/
	SQLFiles

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "\033[0m"

;;
4*)
	SFROOT=$PWD

	cd $SFROOT/share/locale/europe/quest/

	LIST_FILE='quest_list'
	if [ -r $LIST_FILE ]; then
		rm -rdf object
		mkdir object

		while read line;
			do
				./qc $line
			done < $LIST_FILE
	else
		echo $LIST_FILE' dosyasi bulunamadı'
	fi
;;
5*)
	echo -e "\033[32m"

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "# Yedekleme işlemi başlatılıyor."
	echo -e "# ---------------------------------------------------------------------------- #"
	sleep 2

	# Ana yedek dizini
	BACKUP_BASE_DIR="/usr/beststudio_yedek"

	# Tarihli alt klasör ismi (dakika için %M kullan)
	DATE_STR=$(date +%d.%m.%Y_%H.%M)
	TARGET_DIR="${BACKUP_BASE_DIR}/${DATE_STR}"

	# Klasörleri oluştur
	mkdir -p "${TARGET_DIR}"

	# game yedeği: best_game.tar.gz
	tar cfzv "${TARGET_DIR}/best_game.tar.gz" /usr/home
	sleep 2

	# mysql yedeği: best_mysql.tar.gz
	tar cfzv "${TARGET_DIR}/best_mysql.tar.gz" /var/db/mysql
	sleep 2

	echo -e "# Yedek dosyaları ${TARGET_DIR} dizininde bulunabilir."
	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "\033[0m"
;;
6*)
	echo -e "\033[32m"

	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "# İşlem başarıyla iptal edildi."
	echo -e "# ---------------------------------------------------------------------------- #"
	echo -e "\033[0m"
;;
esac
