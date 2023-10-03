#include <CL/cl.h>
#include <libclew/ocl_init.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>


template<typename T>
std::string to_string(T value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

void reportError(cl_int err, const std::string &filename, int line) {
    if (CL_SUCCESS == err)
        return;

    // Таблица с кодами ошибок:
    // libs/clew/CL/cl.h:103
    // P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
    std::string message = "OpenCL error code " + to_string(err) + " encountered at " + filename + ":" + to_string(line);
    throw std::runtime_error(message);
}

#define OCL_SAFE_CALL(expr) reportError(expr, __FILE__, __LINE__)


int main() {
    // Пытаемся слинковаться с символами OpenCL API в runtime (через библиотеку libs/clew)
    if (!ocl_init())
        throw std::runtime_error("Can't init OpenCL driver!");

    // Откройте
    // https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/
    // Нажмите слева: "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformIDs"
    // Прочитайте документацию clGetPlatformIDs и убедитесь, что этот способ узнать, сколько есть платформ, соответствует документации:
    cl_uint platformsCount = 0;
    OCL_SAFE_CALL(clGetPlatformIDs(0, nullptr, &platformsCount));
    std::cout << "Number of OpenCL platforms: " << platformsCount << std::endl;

    // Тот же метод используется для того, чтобы получить идентификаторы всех платформ - сверьтесь с документацией, что это сделано верно:
    std::vector<cl_platform_id> platforms(platformsCount);
    OCL_SAFE_CALL(clGetPlatformIDs(platformsCount, platforms.data(), nullptr));

    for (int platformIndex = 0; platformIndex < platformsCount; ++platformIndex) {
        std::cout << "Platform #" << (platformIndex + 1) << "/" << platformsCount << std::endl;
        cl_platform_id platform = platforms[platformIndex];

        // Откройте документацию по "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformInfo"
        // Не забывайте проверять коды ошибок с помощью макроса OCL_SAFE_CALL
        size_t platformNameSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &platformNameSize));
        // TODO 1.1
        // Попробуйте вместо CL_PLATFORM_NAME передать какое-нибудь случайное число - например 239
        // Т.к. это некорректный идентификатор параметра платформы - то метод вернет код ошибки
        // Макрос OCL_SAFE_CALL заметит это, и кинет ошибку с кодом
        // Откройте таблицу с кодами ошибок:
        // libs/clew/CL/cl.h:103
        // P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
        // Найдите там нужный код ошибки и ее название
        // Затем откройте документацию по clGetPlatformInfo и в секции Errors найдите ошибку, с которой столкнулись
        // в документации подробно объясняется, какой ситуации соответствует данная ошибка, и это позволит, проверив код, понять, чем же вызвана данная ошибка (некорректным аргументом param_name)
        // Обратите внимание, что в этом же libs/clew/CL/cl.h файле указаны всевоможные defines, такие как CL_DEVICE_TYPE_GPU и т.п.

        // DONE
        // Попробовал 2023 - получил ошибку -30 (CL_INVALID_VALUE)


        // TODO 1.2
        // Аналогично тому, как был запрошен список идентификаторов всех платформ - так и с названием платформы, теперь, когда известна длина названия - его можно запросить:
        std::vector<unsigned char> platformName(platformNameSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, platformNameSize, platformName.data(), &platformNameSize));
        std::cout << "    Platform name: " << platformName.data() << std::endl;

        // TODO 1.3
        // Запросите и напечатайте так же в консоль вендора данной платформы
        size_t vendorNameSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0, nullptr, &vendorNameSize));
        std::vector<unsigned char> vendorName(vendorNameSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, vendorNameSize, vendorName.data(), &vendorNameSize));
        std::cout << "    Vendor name: " << vendorName.data() << std::endl;

        // TODO 2.1
        // Запросите число доступных устройств данной платформы (аналогично тому, как это было сделано для запроса числа доступных платформ - см. секцию "OpenCL Runtime" -> "Query Devices")
        cl_uint devicesCount = 0;
        OCL_SAFE_CALL(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &devicesCount));
        std::cout << "    Number of devices: " << devicesCount << std::endl;

        std::vector<cl_device_id> devices(devicesCount);
        OCL_SAFE_CALL(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, devicesCount, devices.data(), &devicesCount));

        for (int deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
                // TODO 2.2
                // Запросите и напечатайте в консоль:
                // - Название устройства
                // - Тип устройства (видеокарта/процессор/что-то странное)
                // - Размер памяти устройства в мегабайтах
                // - Еще пару или более свойств устройства, которые вам покажутся наиболее интересными
                cl_device_id device = devices[deviceIndex];
                std::cout << "    Device #" << (deviceIndex + 1) << "/" << devicesCount << std::endl;

                // Name CL_DEVICE_NAME
                size_t nameSize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &nameSize));
                std::vector<unsigned char> deviceName(nameSize);
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_NAME, nameSize, deviceName.data(), &nameSize));
                std::cout << "        Device Name: " << deviceName.data() << std::endl;

                // Type CL_DEVICE_TYPE
                size_t typeSize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_TYPE, 0, nullptr, &typeSize));
                cl_device_type deviceType;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_TYPE, typeSize, &deviceType, 0));
                std::cout << "        Device Type: ";

                // From cl.h
                if (deviceType == 1 << 1) {
                    std::cout << "CPU" << std::endl;
                } else if (deviceType == 1 << 2) {
                    std::cout << "GPU" << std::endl;
                } else if (deviceType == 1 << 3) {
                    std::cout << "Accelerator" << std::endl;
                }

                // Memory in MB CL_DEVICE_GLOBAL_MEM_SIZE
                // Размер памяти перевожу из байт в Мб
                size_t memorySizeSize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, 0, nullptr, &memorySizeSize));
                cl_ulong memorySize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, memorySizeSize, &memorySize, 0));
                std::cout << "        Memory Size (MB): " << memorySize / 1024 / 1024 << std::endl;

                // Max compute units CL_DEVICE_MAX_COMPUTE_UNITS
                // Как я понимаю это количество ядер на процессоре или количество work group на видеокарте
                size_t maxComputeUnitsSize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, 0, nullptr, &maxComputeUnitsSize));
                cl_ulong maxCopmuteUnits = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, maxComputeUnitsSize, &maxCopmuteUnits, 0));
                std::cout << "        Max compute units: " << maxCopmuteUnits << std::endl;

                // Max number of work items in a work group CL_DEVICE_MAX_WORK_GROUP_SIZE
                // Максимальное количество work-item'ов в одной work-group'е
                size_t maxGroupSizeSize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, 0, nullptr, &maxGroupSizeSize));
                size_t maxGroupSize = 0;
                OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, maxGroupSizeSize, &maxGroupSize, 0));
                std::cout << "        Max group size: " << maxGroupSize << std::endl;
            }
        }

        return 0;
}
