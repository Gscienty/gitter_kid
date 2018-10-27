import React, { Component } from 'react';
import {
    Navbar,
    NavLink,
    NavbarBrand,
    NavbarToggler,
    Nav,
    NavItem,
    Collapse,
    InputGroup,
    InputGroupAddon,
    Button,
    Input
} from 'reactstrap';

export default class Head extends Component {
    constructor(props) {
        super(props);

        this.state = {
            isOpen: false
        };
    }

    toggle() {
        this.setState({ isOpen: !this.state.isOpen });
    }

    unloginRender() {
        return <Collapse navbar>
            <Nav className="ml-auto" navbar>
                <NavItem><NavLink>登录</NavLink></NavItem>
                <NavItem><NavLink>注册</NavLink></NavItem>
            </Nav>
        </Collapse>;
    }

    loginRender() {
        return <Collapse navbar>
            <Nav className="ml-auto" navbar>
                <NavItem><NavLink>Hello ! { this.props.username }</NavLink></NavItem>
                <NavItem><NavLink>注销</NavLink></NavItem>
            </Nav>
        </Collapse>;
    }

    render() {
        return <div>
            <Navbar color="light" light expand="md">
                <NavbarBrand>Gitter Kid</NavbarBrand>
                <Collapse navbar>
                    <Nav navbar>
                        <NavItem>
                            <InputGroup>
                                <Input />
                                <InputGroupAddon addonType="append">
                                    <Button outline color="info" style={{ border: '1px solid #ced4da' }}>查询</Button>
                                </InputGroupAddon>
                            </InputGroup>
                        </NavItem>
                    </Nav>
                </Collapse>
                { this.props.isLogin ? this.loginRender() : this.unloginRender() }
            </Navbar>
        </div>
    }
};
