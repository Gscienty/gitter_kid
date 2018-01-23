package indi.gscienty.gitterkid.webapi.controllers;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping(value = { "/sysinfo" })
public class VersionController {
	
	@RequestMapping(value = { "/version" }, method = RequestMethod.GET)
	public String getVersion() {
		return "0.1 (alpha)";
	}
}
