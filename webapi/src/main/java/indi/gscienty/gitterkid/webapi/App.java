package indi.gscienty.gitterkid.webapi;

import org.springframework.beans.BeansException;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.ApplicationContext;

/**
 * Hello world!
 *
 */
@SpringBootApplication
public class App
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
}
