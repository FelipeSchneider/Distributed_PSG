%clear all;  clc;
close all; 
%% mudar somente estas variáveis
paciente = 5;
hora = 3;
plot_all = 0; %plota todas as variaveis em diferentes graficos se = a 1

%% Inicialização das variáveis
bytes = 0;
raw_acc_x = 0; raw_acc_y = 0;  raw_acc_z = 0;
raw_bat = 0;

rec_acc_x = 0;rec_acc_y = 0;rec_acc_z = 0;
rec_bat = 0;
%valores adquiridos em teste
zero_g_x = 2249.4; um_g_x = 1851.7 ; coef_a_g_x = 9.81/(um_g_x-zero_g_x); %um_g_x = 2647.1
zero_g_y = 2224.6; um_g_y = 1820.0; coef_a_g_y = 9.81/(um_g_y-zero_g_y);
zero_g_z = 2312.8; um_g_z = 2702.9; coef_a_g_z = 9.81/(um_g_z-zero_g_z);

one_second_MD_ACC = 246;

%% Abrindo o arquivo
disp('Abrindo o arquivo')
file_name = sprintf('./files_ACC/ACCP%dH%02d.RAW',paciente,hora);
fileID = fopen(file_name);
if(fileID == -1)
    disp('Arquivo não encontrado')
    return;
else
    rec_message = fread(fileID,'uint8');
end
fclose('all');
size_message = length(rec_message);
segundos = round(size_message/one_second_MD_ACC);
msg = sprintf('Esta placa armazenou %d segundos',segundos);
disp(msg)

%% Decodificando o arquivo
cont_exit = 1;
headers(1) = 0; headers(2) = 0;
while(cont_exit<=length(rec_message))
    f_headers_ok = 0;
    cont_problem = 0; jump_problem = 0;
    while (f_headers_ok == 0) %procurando pelos headers
        headers(1) = headers(2);
        headers(2) = rec_message(cont_exit);
        cont_exit = cont_exit + 1;
        cont_problem = cont_problem+1;
        if((headers(1)==83)&&(headers(2)==84))  
                f_headers_ok = 1;
        end
        if (cont_problem > (3+jump_problem))
           disp(sprintf('Segundo %d NÃO está OK, problema com os headers',round(cont_exit/one_second_MD_EOG)));
           cont_problem = 0; jump_problem = 200;
        end
    end %end of checking headers situation
    
    %lendo um segundo de gravação
    buffer_rec = rec_message(cont_exit:cont_exit + one_second_MD_ACC-3);
    cont_exit = cont_exit + one_second_MD_ACC-2;
    %checando a situação dos CRC, +83+84 representa os headers "ST"
    sum_check = double(sum(double(buffer_rec(1:one_second_MD_ACC-4)))) + 83 +84;
    div = floor(sum_check/65536); %o matlab satura em 65535
    crc_check = sum_check - div*65536;%precisamos então dividir e subtrair
    CRC_1 = double(buffer_rec(one_second_MD_ACC-3)); CRC_2 = double(buffer_rec(one_second_MD_ACC-2));
    CRC = CRC_1*2^8+CRC_2+1; %(+1) pois o programa inicia a soma com 0xffff
%     disp(sprintf('CRC = %d; crc_check = %d;',CRC, crc_check))
%     CRC = crc_check;
    if(crc_check == CRC)
        %separing the data into different buffers if CRC is correct
        %disp(sprintf('Segundo %d está OK',round(cont_exit/one_second_MD_ACC))); 
        raw_acc_x = buffer_rec(1:80); 
        raw_acc_y = buffer_rec(81:160);  
        raw_acc_z = buffer_rec(161:240);
        raw_bat = buffer_rec(241:242);
    else
        %making all equals zero if CRC is wrong
        disp(sprintf('Segundo %d NÃO está OK',round(cont_exit/one_second_MD_ECG))); 
%         raw_acc_x = buffer_rec(1201:1280).*0; 
%         raw_acc_y = buffer_rec(1281:1360).*0;  
%         raw_acc_z = buffer_rec(1361:1440).*0;
%         raw_bat = buffer_rec(1921:1922).*0;
    end
    
    rec_acc_x = [rec_acc_x;swapbytes(typecast(uint8(raw_acc_x), 'uint16'))];
    rec_acc_y = [rec_acc_y;swapbytes(typecast(uint8(raw_acc_y), 'uint16'))];
    rec_acc_z = [rec_acc_z;swapbytes(typecast(uint8(raw_acc_z), 'uint16'))];
    rec_bat = [rec_bat;swapbytes(typecast(uint8(raw_bat), 'uint16'))];
end
rec_bat(1) = [];
rec_acc_x(1) = [];rec_acc_y(1) = [];rec_acc_z(1) = [];
rec_acc_x = double(rec_acc_x); rec_acc_y = double(rec_acc_y);
rec_acc_z = double(rec_acc_z);

g_acc_x = (rec_acc_x-zero_g_x).*coef_a_g_x; g_acc_y = (rec_acc_y-zero_g_y).*coef_a_g_y;
g_acc_z = (rec_acc_z-zero_g_z).*coef_a_g_z;

[b,a] = butter(2,0.25,'low');
Dump = ones(50000,1); Dump = Dump.*g_acc_x(1);
filt_acc_x_acc = [Dump;g_acc_x]; filt_acc_x_acc = filter(b,a,filt_acc_x_acc);
filt_acc_x_acc = filt_acc_x_acc(50001:end);

Dump = ones(50000,1); Dump = Dump.*g_acc_y(1);
filt_acc_y_acc = [Dump;g_acc_y]; filt_acc_y_acc = filter(b,a,filt_acc_y_acc);
filt_acc_y_acc = filt_acc_y_acc(50001:end);

Dump = ones(50000,1); Dump = Dump.*g_acc_z(1);
filt_acc_z_acc = [Dump;g_acc_z]; filt_acc_z_acc = filter(b,a,filt_acc_z_acc);
filt_acc_z_acc = filt_acc_z_acc(50001:end);


%% Plots
acc_time = 0:1/40:segundos-1/40; acc_time = acc_time';

if(plot_all ~=0)
    figure; 
    plot(double(rec_bat)/100);ylabel('Bateria [V]');
    figure; 
    subplot(3,1,1);plot(acc_time,g_acc_x);ylabel('Eixo X [m/s²]');grid on; title('Acelerometros'); 
    subplot(3,1,2);plot(acc_time,g_acc_y);ylabel('Eixo Y [m/s²]');grid on; %set(gca,'XTick',[]); 
    subplot(3,1,3);plot(acc_time,g_acc_z);ylabel('Eixo Z [m/s²]'); grid on;
end

figure; 
subplot(2,2,1); plot(double(rec_bat)/100);ylabel('Bateria [V]'); grid on; axis([0 segundos -inf inf])
subplot(2,2,2); plot(acc_time,filt_acc_x_acc);ylabel('Eixo X [m/s²]'); grid on; xlabel('Tempo [s]'); axis([0 segundos -inf inf])
subplot(2,2,3); plot(acc_time,filt_acc_y_acc);ylabel('Eixo Y [m/s²]');grid on; axis([0 segundos -inf inf])
subplot(2,2,4); plot(acc_time,filt_acc_z_acc);ylabel('Eixo Z [m/s²]');grid on; xlabel('Tempo [s]');axis([0 segundos -inf inf])
suptitle(sprintf('MD-ACC Paciente%d, Hora %02d',paciente,hora));