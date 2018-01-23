package indi.gscienty.gitterkid.webapi;

import java.net.InetAddress;
import java.net.UnknownHostException;

import org.springframework.beans.BeansException;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.embedded.ConfigurableEmbeddedServletContainer;
import org.springframework.boot.context.embedded.EmbeddedServletContainerCustomizer;
import org.springframework.context.ApplicationContext;

/**
 * Hello world!
 *
 */
@SpringBootApplication
public class App implements EmbeddedServletContainerCustomizer
{
	private static ApplicationContext context;
	
    public static void main( String[] args )
    {
    	App.context = SpringApplication.run(App.class, args);
    }
    
    public static <T> T getBean(Class<T> type) throws BeansException {
    	if (App.context == null) {
    		throw new RuntimeException("spring have not initialized");
    	}
    	
    	return App.context.getBean(type);
    }

	@Override
	public void customize(ConfigurableEmbeddedServletContainer container) {
		try {
			
			container.setAddress(InetAddress.getByAddress(new byte[] {0, 0, 0, 0}));
			container.setPort(8080);
			
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
		
		
	}
}
