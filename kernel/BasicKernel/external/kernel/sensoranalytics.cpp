#include "sensoranalytics.h"

#include <QtNetwork/QNetworkInterface>
#include <QtCore/QStandardPaths>

#include "sensors_analytics_sdk.h"
#include "interface/appsettinginterface.h"

namespace creative_kernel
{
	SensorAnalytics::SensorAnalytics(QObject* parent)
		: QObject(parent)
	{

	}

	SensorAnalytics::~SensorAnalytics()
	{

	}

	void SensorAnalytics::trace(const QString& eventType, const QString& eventAction)
	{
        bool rst = initSDK();
        if (!rst)
            return;

        // ���ù������ԣ���Щ���Խ��Զ�������ÿ����Ϊ�¼���������
        sensors_analytics::PropertiesNode super_properties;
        super_properties.SetString("app_name", "Creality Print");
        std::string platformstr = "Win";
#ifdef __APPLE__
        platformstr = "Mac";
#endif
        super_properties.SetString("platform", platformstr.c_str());
        sensors_analytics::Sdk::RegisterSuperProperties(super_properties);

        // ��¼һ����Ϊ�¼�
        sensors_analytics::PropertiesNode event_properties;
        event_properties.SetString(eventType.toStdString(), eventAction.toStdString());

        // ����������㶼û���������͵�����ˣ����������ʱ������� Flush �ֹ���������
        // ע�⣺�������� Flush �����Żᴥ�����緢��
        // �����������ģ����Կ���ʹ�ö����̵߳��÷��ͺ���
        // �����Ϊ�������ⷢ��ʧ�ܣ���������ֵΪ false
        sensors_analytics::Sdk::Flush();

        // ���̽���ǰû�� Flush �����ݽ����浽 staging_file
        sensors_analytics::Sdk::Track("BuyTicket");
        sensors_analytics::Sdk::Shutdown();
	}

    QString SensorAnalytics::getPCMacAddress()
    {
        QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
        int nCnt = nets.count();
        QString strMacAddr = "";
        for (int i = 0; i < nCnt; i++)
        {
            if (nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
            {
                strMacAddr = nets[i].hardwareAddress();
                break;
            }
        }
        return strMacAddr;
    }

    bool SensorAnalytics::initSDK()
    {
        // �ݴ��ļ�·�������ļ����ڽ����˳�ʱ���ڴ���δ���͵������ݴ��ڴ��̣��´η���ʱ����
        QByteArray cdata = ANALYTICS_PATH.toLocal8Bit() + "/staging_file";
        const std::string staging_file_path(cdata);

        // ��������ݽ��յ�ַ
        const std::string server_url = "http://2-model-admin-dev.crealitygroup.com/api/rest/bicollector/front/sa/data";

        // ������� UUID ��Ϊ distinct_id
        // ע�⣺����ֻ����Ϊ demo ��ʾ���������һ�� ID�������ʽʹ�ã���������ʹ��������ʽ���豸 ID�������Լ������������´ι��� SDK ʱʹ��֮ǰ��ͬ�� ID �Ա�ʶͬһ�豸��
        const std::string distinct_id = getPCMacAddress().toStdString();
        //std::cout << "distinct_id: " << distinct_id << std::endl;
        // ��� ID ��Ϊ ���豸 ID�� �� ����¼ ID�� ���֣�������ɵ��� ���豸 ID��
        const bool is_login_id = false;

        // ��������ݴ棨δ���� Flush ���ͣ���������������������ֵʱ�����Ӷ�����̭�ɵ�����
        const int max_staging_record_size = 200;

        // ��ʼ�� SDK
        bool rst = sensors_analytics::Sdk::Init(staging_file_path, server_url, distinct_id, is_login_id, max_staging_record_size);
        if (rst)
        {
            sensors_analytics::Sdk::EnableLog(true);
        }

        return rst;
    }
}