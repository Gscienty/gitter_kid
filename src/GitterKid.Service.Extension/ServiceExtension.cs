using Microsoft.Extensions.DependencyInjection;

namespace GitterKid.Service.Extension
{
    public static class ServiceExtension
    {
        public static void AddGitterKidService(this IServiceCollection service, string repositorysPath)
        {
            service.AddSingleton<RepositoryFactory>(provider => new RepositoryFactory(repositorysPath));
        }
    }
}