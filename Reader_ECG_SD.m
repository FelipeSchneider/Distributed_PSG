clear all;  %clc;
close all; 
%% Inicialização das variáveis
%para ler diferentes arquivos, mudar estas 3 variaveis
paciente = 2;
hora = 1;
plot_all = 1; %plota todas as variaveis em diferentes graficos se = a 1

bytes = 0;
raw_ecg = 0; raw_acc_x = 0; raw_acc_y = 0;  raw_acc_z = 0;
raw_bat = 0; raw_acc_2_x = 0; raw_acc_2_y = 0;  raw_acc_2_z = 0;
raw_acc_3_x = 0; raw_acc_3_y = 0;  raw_acc_3_z = 0;

rec_acc_x = 0;rec_acc_y = 0;rec_acc_z = 0;
rec_acc_2_x = 0;rec_acc_2_y = 0;rec_acc_2_z = 0;
rec_acc_3_x = 0;rec_acc_3_y = 0;rec_acc_3_z = 0;
rec_bat = 0;rec_ecg = 0;
%valores adquiridos em teste
zero_g_x = 2240; um_g_x = 2625; coef_a_g_x = 9.8/(um_g_x-zero_g_x);
zero_g_y = 2205; um_g_y = 2609; coef_a_g_y = 9.8/(um_g_y-zero_g_y);
zero_g_z = 2314; um_g_z = 2684; coef_a_g_z = 9.8/(um_g_z-zero_g_z);

one_second_MD_ECG = 1926;

%% Abrindo o arquivo
disp('Abrindo o arquivo')
file_name = sprintf('./files_ECG/ECGP%dH%02d.RAW',paciente,hora);
fileID = fopen(file_name);
if(fileID == -1)
    disp('Arquivo não encontrado')
    break;
else
    rec_message = fread(fileID,'uint8');
end
fclose('all');
size_message = length(rec_message);
segundos = round(size_message/one_second_MD_ECG);
msg = sprintf('Esta placa armazenou %d segundos',segundos);
disp(msg);

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
    buffer_rec = rec_message(cont_exit:cont_exit + one_second_MD_ECG-3);
    cont_exit = cont_exit + one_second_MD_ECG-2;
    %checando a situação dos CRC, +83+84 representa os headers "ST"
    sum_check = double(sum(double(buffer_rec(1:one_second_MD_ECG-4)))) + 83 +84;
    div = floor(sum_check/65536); %o matlab satura em 65535
    crc_check = sum_check - div*65536;%precisamos então dividir e subtrair
    CRC_1 = double(buffer_rec(one_second_MD_ECG-3)); CRC_2 = double(buffer_rec(one_second_MD_ECG-2));
    CRC = CRC_1*2^8+CRC_2+1; %(+1) pois o programa inicia a soma com 0xffff
%     disp(sprintf('CRC = %d; crc_check = %d;',CRC, crc_check))
%     CRC = crc_check;
    if(crc_check == CRC)
        %separing the data into different buffers if CRC is correct
        %disp(sprintf('Segundo %d está OK',round(cont_exit/one_second_MD_ECG)));
        raw_ecg = uint32(buffer_rec(1:1200));  
        raw_acc_x = buffer_rec(1201:1280); 
        raw_acc_y = buffer_rec(1281:1360);  
        raw_acc_z = buffer_rec(1361:1440);
        raw_acc_2_x = buffer_rec(1441:1520); 
        raw_acc_2_y = buffer_rec(1521:1600);  
        raw_acc_2_z = buffer_rec(1601:1680);
        raw_acc_3_x = buffer_rec(1681:1760); 
        raw_acc_3_y = buffer_rec(1761:1840);  
        raw_acc_3_z = buffer_rec(1841:1920);
        raw_bat = buffer_rec(1921:1922);
    else
        %making all equals zero if CRC is wrong
        disp(sprintf('Segundo %d NÃO está OK',round(cont_exit/one_second_MD_ECG)));
%         raw_ecg = uint32(buffer_rec(1:1200)).*0;  
%         raw_acc_x = buffer_rec(1201:1280).*0; 
%         raw_acc_y = buffer_rec(1281:1360).*0;  
%         raw_acc_z = buffer_rec(1361:1440).*0;
%         raw_acc_2_x = buffer_rec(1441:1520).*0; 
%         raw_acc_2_y = buffer_rec(1521:1600).*0;  
%         raw_acc_2_z = buffer_rec(1601:1680).*0;
%         raw_acc_3_x = buffer_rec(1681:1760).*0; 
%         raw_acc_3_y = buffer_rec(1761:1840).*0;  
%         raw_acc_3_z = buffer_rec(1841:1920).*0;
%         raw_bat = buffer_rec(1921:1922).*0;
    end

    %converting the inputs in the original formats (uint16 or 32)
    for i=1:400
        aux_ecg(i,1) = (raw_ecg(i*3-2)*2^16+ raw_ecg(i*3-1)*2^8+ raw_ecg(i*3));
    end
    rec_ecg = [rec_ecg;aux_ecg];
    rec_acc_x = [rec_acc_x;swapbytes(typecast(uint8(raw_acc_x), 'uint16'))];
    rec_acc_y = [rec_acc_y;swapbytes(typecast(uint8(raw_acc_y), 'uint16'))];
    rec_acc_z = [rec_acc_z;swapbytes(typecast(uint8(raw_acc_z), 'uint16'))];
    
    rec_acc_2_x = [rec_acc_2_x;swapbytes(typecast(uint8(raw_acc_2_x), 'uint16'))];
    rec_acc_2_y = [rec_acc_2_y;swapbytes(typecast(uint8(raw_acc_2_y), 'uint16'))];
    rec_acc_2_z = [rec_acc_2_z;swapbytes(typecast(uint8(raw_acc_2_z), 'uint16'))];
    
    rec_acc_3_x = [rec_acc_3_x;swapbytes(typecast(uint8(raw_acc_3_x), 'uint16'))];
    rec_acc_3_y = [rec_acc_3_y;swapbytes(typecast(uint8(raw_acc_3_y), 'uint16'))];
    rec_acc_3_z = [rec_acc_3_z;swapbytes(typecast(uint8(raw_acc_3_z), 'uint16'))];
    
    rec_bat = [rec_bat;swapbytes(typecast(uint8(raw_bat), 'uint16'))];
end
%% Ajuste dos vetores e filtros 
%excluindo o zero da primeira posição
rec_acc_x(1) = [];rec_acc_y(1) = [];rec_acc_z(1) = [];
rec_acc_2_x(1) = [];rec_acc_2_y(1) = [];rec_acc_2_z(1) = [];
rec_acc_3_x(1) = [];rec_acc_3_y(1) = [];rec_acc_3_z(1) = [];
rec_bat(1) = []; rec_ecg(1) = [];

rec_acc_x = double(rec_acc_x); rec_acc_y = double(rec_acc_y);
rec_acc_z = double(rec_acc_z);
rec_acc_2_x = double(rec_acc_2_x); rec_acc_2_y = double(rec_acc_2_y);
rec_acc_2_z = double(rec_acc_2_z);
rec_acc_3_x = double(rec_acc_3_x); rec_acc_3_y = double(rec_acc_3_y);
rec_acc_3_z = double(rec_acc_3_z);

g_acc_x = (rec_acc_x-zero_g_x).*coef_a_g_x; g_acc_y = (rec_acc_y-zero_g_y).*coef_a_g_y;
g_acc_z = (rec_acc_z-zero_g_z).*coef_a_g_z;
g_acc_2_x = (rec_acc_2_x-zero_g_x).*coef_a_g_x; g_acc_2_y = (rec_acc_2_y-zero_g_y).*coef_a_g_y;
g_acc_2_z = (rec_acc_2_z-zero_g_z).*coef_a_g_z;
g_acc_3_x = (rec_acc_3_x-zero_g_x).*coef_a_g_x; g_acc_3_y = (rec_acc_3_y-zero_g_y).*coef_a_g_y;
g_acc_3_z = (rec_acc_3_z-zero_g_z).*coef_a_g_z;

rec_ecg = double(rec_ecg);
[b,a] = butter(2,0.0005,'high');
Dump = ones(100000,1); Dump = Dump.*rec_ecg(1);
ecg_filt = [Dump;rec_ecg]; ecg_filt = filter(b,a,ecg_filt);
ecg_filt = ecg_filt(100001:end);

[b,a] = butter(2,0.25,'low');
Dump = ones(50000,1); Dump = Dump.*g_acc_x(1);
filt_acc_x = [Dump;g_acc_x]; filt_acc_x = filter(b,a,filt_acc_x);
filt_acc_x = filt_acc_x(50001:end);

Dump = ones(50000,1); Dump = Dump.*g_acc_y(1);
filt_acc_y = [Dump;g_acc_y]; filt_acc_y = filter(b,a,filt_acc_y);
filt_acc_y = filt_acc_y(50001:end);

Dump = ones(50000,1); Dump = Dump.*g_acc_z(1);
filt_acc_z = [Dump;g_acc_z]; filt_acc_z = filter(b,a,filt_acc_z);
filt_acc_z = filt_acc_z(50001:end);


%% Plots
ecg_time = 0:1/400:segundos-1/400; ecg_time = ecg_time';
acc_time = 0:1/40:segundos-1/40; acc_time = acc_time';

if(plot_all ~=0)
    figure; 
    subplot(2,1,1); plot(ecg_time,ecg_filt);title('ECG filtrado'); 
    grid on; ylabel('Valores AD'); xlabel('segundos');
    subplot(2,1,2); plot(ecg_time,rec_ecg);title('ECG não filtrado');ylabel('Valores AD');
    xlabel('segundos'); grid on;
    figure; 
    plot(double(rec_bat)/100);ylabel('Bateria [V]');
    figure; 
    subplot(3,1,1);plot(acc_time,g_acc_x);ylabel('Eixo X [m/s²]');grid on; %set(gca,'XTick',[]); 
    subplot(3,1,2);plot(acc_time,g_acc_y);ylabel('Eixo Y [m/s²]');grid on; %set(gca,'XTick',[]); 
    subplot(3,1,3);plot(acc_time,g_acc_z);ylabel('Eixo Z [m/s²]'); grid on;
end

figure; 
subplot(2,2,1); plot(ecg_time,ecg_filt);ylabel('ECG'); grid on; axis([0 segundos -inf inf])
subplot(2,2,2); plot(acc_time,filt_acc_x);ylabel('Eixo X [m/s²]'); grid on;xlabel('Tempo [s]');axis([0 segundos -inf inf]) 
subplot(2,2,3); plot(acc_time,filt_acc_y);ylabel('Eixo Y [m/s²]');grid on; axis([0 segundos -inf inf])
subplot(2,2,4); plot(acc_time,filt_acc_z);ylabel('Eixo Z [m/s²]');grid on; xlabel('Tempo [s]');axis([0 segundos -inf inf])
suptitle(sprintf('MD-ECG Paciente%d, Hora %02d',paciente,hora));
