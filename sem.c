///////////////////Черновик//////////////////
cd /mnt/c/infa/infa_2year/

gcc -Werror -Wall -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type -g -O2 -fwhole-program 01_stat.c


advanced linux programming -- книжка с разобранными задачами

/////////////////Задачи///////////////////

&1.1 реализовать stat

&2.1 записать в указанный файл указанное содержимое (строку)
open, write, close
&2.2 2.1, не заворачивая в цикл (open, dprintf %s)

cyclomatic complextity почитать

&3.1 cp regular file
writeall (readall не нужен)

&3.2 непоследовательное чтение, запись
реализовать 3.1 через pread pwrite
/*lseek -- перемотка (системный вызов), fseek -- сишная функция
у дескриптора -- позиция в файле
копирование с перестановкой блоков или разобрать иерарическое хранилище
история браузера -- там есть база; sqlite
вывести корневой каталог*/

&3.3 (3*) скопировать другие файлы: regular file, FIFO, symlink, block/character device
mknod -- создавать block или character device; 
pipe -- труба; fifo -- именованная труба
в FIFO содержимого нет, создать FIFO с именем argv[2] : mkfifo argv[2]

coreutils   busybox

if (filetype == symlink) {
	readlink(where to read в буфер, where to write), symlink
}

&4. доработать задачу 3
нужно копировать не только данные, но и некоторые метаданные (права, время доступа и модификации atime, mtime)
chmod -- по адресу файла
fchmod -- по fd
скопировать файл, потом изменить права как у оригинала
не передавать права в open!!!
futimens (dst_fd, const struct timespec[2] {sb.st_atim, sb.st_mtim})

&5*. задача 4 + копируем остальные метаданные (uid, gid)
если номер устройства и номер узла совпадают, то это ссылки на один и тот же файл
rsynk -- полезная штука почитать -avxHAXS  --prallocate
Uid: если не одинаковый, то нужно из под более влиятельного пользователя (с повышенными привилегиями) зайти и изменить Uid
chown, fchown, lchown
ACLs
xattrs

&6.1 упрощенный ls -la(в т ч скрытые файлы): вывести список записей в текущем каталоге без рекурсии
вывести только тип записи, имя записи:
d Downloads
- filefj
d hgfjghfkj
d fhlhl

opendir/readdir/closedir 
lstat для типа

&6.2 (6.1 с argv[1]) ls -la -- для текущего каталога либо для argv[1], если указан 
opendir+dirfd, fstat, readdir, closedir
Проверить, что передан каталог
Можно использовать fstatat; либо в начале проверка argc = 3 и cd в argv[1] и 6.1

6.3 6.1 через open, getdents, close
меньше вызовов функций!!! хотим прочесть все записи за один вызов; //readdir -- не вызов, так что ок
getdents(64) считывает в массив байтов структур переменной длины
нельзя завести массив структур, так как в буфере выделяется места 0 на переменную часть
есть в мане!!!!!!!! 
нельзя обратиться к структуре[i], так как у них разные длины
ускорение чтения больших каталогов

6.4 рекурсивный 6.2  if тип записи == d, то открыть его;
перед выводом количество пробелов == глубине рекурсии
обход в глубину

7.1 cp -a копирование директорий copy dir без рекурсии
7.2 с рекурсией


8* физическое размещение файла на диске
filefrag (hdparm --fibmap)
дефрагментация

9. вывести занятое, свободное и доступное (для использования) пространство для 
файловой системы, в которой лежит указанный файл или каталог
statfs(linux specific) / statvfs

9*. разным uid разное место
quota/quotactl

10. (овсянникова) напечатать информацию о появлении новых файлов в указанном каталоге, мы их отслеживаем с помощью:
решать через inotify (fanotify, dnotify, kqueue, ...) -- механизм 
создается очередь событий
! переименованный файл != новый файл (они по разному кодируются)

11. файловые блокировки
программа, которая атомарно подсчитывает и выводит количество запусков в текстовом (fscanf) виде (считая текущий) в файле counter.txt;  корректно обрабатывать параллельные запуски многих экземпляров
нужно рядом хранить файлик с 
advisory locks (рекомендательные)
mandatory (принудительные)
чтобы две программы работали одновременно могли работать с одним файлом, они должны договориться
блокировки shared(reader lock) -- неисключающая, exclusive (writer lock) -- исключающая. Читателей м б несколько, писатель -- только один
flock не использовать, использовать lockf
man 2 flock 
- устанавливает блокировку на весь файл, а не на отдельные его части
- работает не на всех файловых системах (не работает на NFS)
- если снимает блокировку, то слетают все блокировки (семантика разблокировки файлового дескриптора)
lockf нужно покрутить с кареткой
- можно блокировать области, тогда можно несколько писателей, которые пишут в разные участки файла

fcntl-lock
// процессы
PID -- process identifier (ID)
	находит место в таблице, кусок памяти, куда программа будет класть свои данные; определяет стандартные потоки ввода и вывода; передает управление на точку входа в программу
состояния процесса: готов к исполнению, спит, исполняется, зомби (процесс завершился и ждет, пока родитель обратится за кодом возврата, тогда полностью завершается)
ps   -- показывает процессы системы
ps ux -eH
ps -aux -- все процессы

wait, waitpid, wait id

man  7 capabilities -- дополнительные права. Для многих программ нужны root права -- распиливает привилегии rootа на мелкие и их дает кому нужно
SELinux
SELinuxlabel -- ругается, если неправильные права, но ничего не запрещает (ну, можно включить такой режим)

Лимиты по ресурсам -- сколько памяти и времени может занять
coredump
getrlimit, setrlimit

&12. вывести информацию про текущий процесс (про запущенный экземпляр этой программы)
id
выводить по порядку как в credentials(7) -- иерархия процессов, группы (по кучкам)
//su
getgroups

&13.1 wait -- дожидается завершения дочернего процесса, waitpid дожидается изменения состояния дочернего процесса
программа порождает дочерний процесс и ждет его завершения, затем выводит информацию о завершении дочернего процесса
пример в man 2 waitpid
13.2 программа порождает дочерний процесс, он дожидается завершения родительского процесса 
//мониторить parent_pid, если он изменился, то родитель умер busybox
parent pid на 1 меньше текущего, когда родитель жив 
когда parent умирает, pid дочернего меняется
ptrace -- 
pidfd_open -- самый короткий, но linux specific, можно его
wait нельзя использовать

fork - создает новый процесс
clone (2) - копия исходного процесса

fork + execve
posix_spawn

&15.1 программа, которая сопротивляется своему завершению 
назначить обработчиков сигналов sigaction (signal устаревший, его не надо использовать)
из другого терминала посылать сигналы:
kill -s signal pid -- послать сигнал signal процессу pid
15.2  задаче 10 добавить корректное завершение работы

15* signalfd/sigqueue

&14.1 dup2(oldfd, newfd) -- в дескриптор newfd (первый свободный) подсунуть копию файла oldfd
&14.2 execve    execlp
&14.3 вывести то же самое, что last | wc -l
pipe  
(на основе 14.1) реализовать использование других программ из своей
соединить stdout одной программы с stdin другой программы(команды пишут на вход друг другу)

Семантика блокировок пайпов :
-если мы читаем из пайпа и там что то лежит, то мы это сразу прочтем, то есть заберем, так как это fifo (прочтем столько, сколько мы запросили, может быть меньше, чем лежало в пайпе)
-если ничего не лежит, то read вернет 0 и можно прекращать читать
если write не хватает места чтобы записать, то он засыпает , получаем сигнал sigpipe (он убивает процесс)
если забудем закрыть дескриптор, то программа будет бесконечно долго ждать данных из stdin, но read не вернет управление, так как все еще есть дескриптор, отвечающий за чтение, и отвечающий за запись, а его некому закрывать
можно модифицировать эту программу: в первом процессы вызвать proc cp info, во втором head, второй словил sigpipe

14.4 poll/epoll/на худой конец select(это старый)
Написать программу, которая в дочернем процессе запускает gzip, работая с ним через два пайпа; померять скорость, с которой gzip сжимает случайные данные
gzip -- сжать
gzip -d -- разжать

upd 20/11 python
import select #есть epoll, poll, select обертки


clock_gettime -- получить разность времен, поделить на него количество закинутых данных -- получится скорость

&16. mq_open/mq_getattr/mq_close/mq_unlink 
Создать очередь сообщений, посмотреть на ее параметры, почистить за сообщений
SysV IPC можно почитать, но это не нужно
коллизия -- разные программы использовали одинаковое имя
Link with -lrt
Очередь -- глобальный объект, его нужно удалить, иначе он будет удален только при перезагрузке
man mq_overview 
17. mq_send/mq_receive 
две программы
одна всегда запущена(сервис) создает очередь сообшений, в бесконечном цикле читает (mq_receive) все сообщения(и время, когда оно прилетело) и печатает (нет \0, печатать printf ом аккуратно); когда приходит сигнал, удаляет очередь
вторая (клиент)-- открывает очередь, посылает туда сообщение, закрывает очередь
сначала закрыть дескриптор очереди, только потом в цикле обрабатывать все сообщения


пайп -- потоковая передача, в каком порядке быди записаны, в таком и прочтены. Хотим отправлять данные сообщениями (чтобы данные не смешивались, если несколько читателей\писателей). Сообщение мы либо прочтем целиком, либо получим ошибку (по кускам нельзя). Очереди сообщений -- глобальные объекты. Будем рассматривать только позиксовые очереди сообщений (в книге карпова конькова разобраны скорее всего только старые)

18. dlopen/dlsym/dlclose

19. потоки
два потока живут "одновременно"
если один поток выбросил ошибку, то все потоки в этом процессе умрут; память общая, то обмен памятью дешевый, так как не надо копировать лишний раз

mutex -- как блокировка для файлов, чтобы последовательно работать с каким то общим объектом; мы должны сами инициализировать операции его захвата и отпускания

два подхода: либо есть атомарные операции (, например сложение целых чисел, тогда их используем), либо их нет (например, сложение дробных чисел, тогда приходится использовать блокировки -- они под капотом используют атомарные операции)

&19.1 pthread_mutex
19.2 безымянный семафор sem_init sem_wait sem_post sem_destroy
19.3 именованный семафор sem_open sem_close sem_ulink
прикрутить к задаче 11 именованные семафоры
&19.4 atomic_fetch_add\atomic_load
работает только с целыми числами

численное интегрирование -- дать кадому потоку свою структуру (разные пределы интегрирования?)

19.5 map-reduce approach
разобъем задачу на подзадачи: каждый поток решает свою подзадачу, пишет в свою область памяти, потом основной поток собирает это все в основное решени
цеикл, в конце join, массив структур work_area
не нужна синхронизация, так как в разных областях памяти

19.6 OpenMP
разветвление языка OpenMP -- встраивает многопоточную работу в стандарт языка, расширяя его синтаксис
сами не создаем и не уничтожаем потоки, не работаем с мютексами, за нас это делает компилятор (за нас вторкает в начале программы работу с потоками, дожидается (join))
код станет короче
скопировать Open MP из туториала, в главный тяжелый цикл написать пару строчек о том как его распараллелить

!пока что не соответствуют номерам файлов:

&20.1 (clock server + clock client) shm_open (-lrt) -- временный именованный файл
shm_close
mmap -- сразу попросить память с нужными правами
память выделяется целыми страницами
MAP_PRIVATE -- для локального процесса
MAP_SHARED -- общая память -- изменения, которые внес дочерний процесс, видны в родительском
struct ... * (см скрин)


20.2 open/fstat/mmap/msync/munmap (in-place encryption/decryption)


///////////////////////Сети
OSI/ISO
https://en.wikipedia.org/wiki/OSI_model
http://kpm8.mipt.ru/sctp.pdf

&21 ip(7), udp(7), bind, nc/socat/netcat

nc -u 93.175.30.74 65530    kpm8 from terminal
nc -u 172.27.185.221 65535  to me from terminal
получить этот номер ip a

s.bind(('0.0.0.0', 7542)) //передаем номер канала
s.bind(('', 0)) //система выделит своболный
del s
s.recvfrom? //подсказка, как man
s.recvfrom(4096) #размер страницы

////////////// send message
10.55.162.34 -- AA

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
s.connect(('93.175.30.74', 65530)) //send message to kpm8
s.send(b'message1')



22 tcp(7)
Cisco Networking Essentials книжка
//curl ident.me
curl ifconfig.me -- узнать свой адрес
rlwrap python3 22_client_new.py -- использует библиотеку readline
23. SCTP












